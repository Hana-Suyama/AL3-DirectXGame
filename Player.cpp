#include "Player.h"
#include <cassert>
#include <MyMath.h>
#include "ImGuiManager.h"

void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {

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
	if (bullet_) {
		bullet_->Update();
	}

	//行列更新
	worldTransform_.UpdateMatrix();
	
	//キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	ImGui::Text("%f. %f. %f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::End();

}

void Player::Draw(ViewProjection& viewProjection_) { 
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	//弾描画
	if (bullet_) {
		bullet_->Draw(viewProjection_);
	}
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
	if (input_->PushKey(DIK_SPACE)) {
		
		//弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);

		//弾を登録
		bullet_ = newBullet;
	}
}
