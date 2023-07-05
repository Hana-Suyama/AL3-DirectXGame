#include "Player.h"
#include <cassert>
#include <MyMath.h>
#include "ImGuiManager.h"

Player::~Player() {
	//bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 pos) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_.z += pos.z;

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("Reticle.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {0, 0}, {1, 1, 1, 1}, {0.5, 0.5});
	
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update(ViewProjection viewProjection) {

	//デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) { 
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	//キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	//キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	////押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	//ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	//スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	//ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
		//スプライトの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition);
	}

	//座標移動(ベクトルの加算)
	worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, move);

	Rotate();

	// 移動限界座標
	const float kMoveLimitX = 300;
	const float kMoveLimitY = 300;

	////範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//キャラクター攻撃処理
	Attack();

	//弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	//行列更新
	worldTransform_.UpdateMatrix();

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	//自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 5.0f;
	//自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = {0, 0, 1.0f};
	//自機のワールド行列の回転を反映
	offset = Multiply(offset, worldTransform_.matWorld_);
	//ベクトルの長さを整える
	offset = Vec3FloatMultiplication(Normalize(offset), kDistancePlayerTo3DReticle);
	//3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_ = Vec3Addition(GetWorldPosition(), offset);
	worldTransform3DReticle_.UpdateMatrix();

	////3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	//Vector3 positionReticle = GetReticleWorldPosition();
	////ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth,WinApp::kWindowHeight,0,1);
	////ビュー行列をプロジェクション行列、ビューポート行列を合成する
	//Matrix4x4 matViewProjectionViewport = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	////ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	//positionReticle = Transform(positionReticle, matViewProjectionViewport);
	////スプライトのレティクルに座標設定
	//sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	//POINT mousePosition;
	//// マウス座標(スクリーン座標)を取得する
	//GetCursorPos(&mousePosition);
	//// クライアントエリア座標に変換する
	//HWND hwnd = WinApp::GetInstance()->GetHwnd();
	//ScreenToClient(hwnd, &mousePosition);

	//sprite2DReticle_->SetPosition({(float)mousePosition.x, (float)mousePosition.y});

	//ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	//合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	//スクリーン座標
	Vector3 posNear = Vector3((float)spritePosition.x, (float)spritePosition.y, 0);
	Vector3 posFar = Vector3((float)spritePosition.x, (float)spritePosition.y, 1);

	//スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	//マウスレイの方向
	Vector3 mouseDirection = Vec3Subtraction(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	//カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 50;
	worldTransform3DReticle_.translation_.x = posNear.x + mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y + mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z + mouseDirection.z * kDistanceTestObject;

	worldTransform3DReticle_.UpdateMatrix();
	
	//キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	ImGui::Text("%f. %f. %f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::End();

	ImGui::Begin("Player");
	ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	ImGui::Text("Near:(%+.2f, %+.2f, %+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+2f, % +.2f, % +.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text(
	    "3DReticle:(% +.2f, % +.2f, % +.2f)", worldTransform3DReticle_.translation_.x,
	    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();

}

void Player::Draw(ViewProjection& viewProjection_) { 
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	//3Dレティクル
	model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_);

	//弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}

void Player::DrawUI() { 
	sprite2DReticle_->Draw();
}

void Player::Rotate() { 
	//回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	Vector3 rotate{};

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		rotate.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		rotate.y += kRotSpeed;
	}

	worldTransform_.rotation_ = Vec3Addition(worldTransform_.rotation_, rotate);
}

void Player::Attack() {

	XINPUT_STATE joyState;

	//ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			//弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity(0, 0, kBulletSpeed);
		
			//速度ベクトルを自機の向きに合わせて回転させる
			//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

			//自機から照準オブジェクトへのベクトル
			velocity.x = GetReticleWorldPosition().x - GetWorldPosition().x;
			velocity.y = GetReticleWorldPosition().y - GetWorldPosition().y;
			velocity.z = GetReticleWorldPosition().z - GetWorldPosition().z;

			velocity = Vec3FloatMultiplication(Normalize(velocity), kBulletSpeed);

			//弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(model_, GetWorldPosition(), velocity);

			//弾を登録
			bullets_.push_back(newBullet);
	}


	//if (input_->PushKey(DIK_SPACE)) {

	//	//弾の速度
	//	const float kBulletSpeed = 1.0f;
	//	Vector3 velocity(0, 0, kBulletSpeed);
	//	
	//	//速度ベクトルを自機の向きに合わせて回転させる
	//	//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	//	//自機から照準オブジェクトへのベクトル
	//	velocity.x = GetReticleWorldPosition().x - GetWorldPosition().x;
	//	velocity.y = GetReticleWorldPosition().y - GetWorldPosition().y;
	//	velocity.z = GetReticleWorldPosition().z - GetWorldPosition().z;

	//	velocity = Vec3FloatMultiplication(Normalize(velocity), kBulletSpeed);

	//	//弾を生成し、初期化
	//	PlayerBullet* newBullet = new PlayerBullet();
	//	newBullet->Initialize(model_, GetWorldPosition(), velocity);

	//	//弾を登録
	//	bullets_.push_back(newBullet);
	//}
}

Vector3 Player::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos{};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetReticleWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {

}

void Player::SetParent(const WorldTransform* parent) {
	//親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
