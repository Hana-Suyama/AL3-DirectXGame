#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include <Enemy.h>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_; 
	//自キャラの解放
	delete player_;

	//天球の開放
	delete skydome_;
	delete modelSkydome_;

	//レールカメラの開放
	delete railCamera_;

	delete debugCamera_;

	// bullet_の解放
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("computer_typing_hayai.png");
	//3Dモデルの生成
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//レティクルのテクスチャ
	TextureManager::Load("Reticle.png");

	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	Vector3 playerPosition(0, 0, 10);
	player_->Initialize(model_, textureHandle_, playerPosition);

	//天球の生成
	skydome_ = new Skydome();
	//天球の初期化
	skydome_->Initialize(modelSkydome_);

	//レールカメラの生成
	railCamera_ = new RailCamera();
	//レールカメラの初期化
	railCamera_->Initialize(worldTransform_.translation_, worldTransform_.rotation_);

	//自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// 敵を生成し、初期化
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(model_, {20, 10, 100}, {0, 0, -0.2f});
	newEnemy->SetGameScene(this);

	// 敵を登録
	// gameScene_->enemyBullets_.push_back(newBullet);
	enemy_.push_back(newEnemy);

	// 敵を生成し、初期化
	/*Enemy* newEnemy2 = new Enemy();
	newEnemy2->Initialize(model_, {-20, 10, 100}, {0, 0, -0.2f});
	newEnemy2->SetGameScene(this);*/

	// 敵を登録
	// gameScene_->enemyBullets_.push_back(newBullet);
	//enemy_.push_back(newEnemy2);

	LoadEnemyPopData();


	//敵キャラに自キャラのアドレスを渡す
	/*for (Enemy* enemy : enemy_) {
		enemy->SetPlayer(player_);
	}*/

}

void GameScene::Update() {

	//敵発生の更新
	UpdateEnemyPopCommands();

	// 敵キャラに自キャラのアドレスを渡す
	for (Enemy* enemy : enemy_) {
		enemy->SetPlayer(player_);
	}

	// 自キャラの更新
	player_->Update(viewProjection_);
	for (Enemy* enemy : enemy_) {
		enemy->SetPlayerPos(player_->GetWorldPosition());
	}
	
	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	
	// デスフラグの立った敵を削除
	enemy_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

	//天球の更新
	skydome_->Update();

	//レールカメラの更新
	railCamera_->Update();

	// NULLポインタチェック
	for (Enemy* enemy : enemy_) {
		assert(enemy);
		enemy->Update();
	}

	CheckAllCollisions();

	// デバッグカメラの更新
	debugCamera_->Update();
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_C)) {
		if (isDebugCameraActive_ == false) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			//ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			//ビュープロジェクション行列の更新と転送
			viewProjection_.UpdateMatrix();
		}

		viewProjection_.matView = railCamera_->GetViewProjection().matView;
	    viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	    viewProjection_.TransferMatrix();

	}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//自キャラの描画
	player_->Draw(viewProjection_);

	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		    bullet->Draw(viewProjection_);
	}

	//天球の描画
	skydome_->Draw(viewProjection_);

	// NULLポインタチェック
	for (Enemy* enemy : enemy_) {
		    assert(enemy);
		    enemy->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
		    posB = bullet->GetWorldPosition();
		    if ((powf(posB.x - posA.x, 2) + powf(posB.y - posA.y, 2) + powf(posB.z - posA.z, 2)) <= powf(2 + 2, 2)) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		    }
	}

	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	// 自キャラの座標
	for (Enemy* enemy : enemy_) {
		    posA = enemy->GetWorldPosition();

			// 自キャラと敵弾全ての当たり判定
			for (PlayerBullet* bullet : playerBullets) {
			posB = bullet->GetWorldPosition();
			if ((powf(posB.x - posA.x, 2) + powf(posB.y - posA.y, 2) + powf(posB.z - posA.z, 2)) <=
				powf(2 + 2, 2)) {
				// 自キャラの衝突時コールバックを呼び出す
				enemy->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}
	
	#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	
	// 自キャラと敵弾全ての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets_) {
			posA = enemyBullet->GetWorldPosition();
			posB = playerBullet->GetWorldPosition();
			if ((powf(posB.x - posA.x, 2) + powf(posB.y - posA.y, 2) + powf(posB.z - posA.z, 2)) <= powf(2 + 2, 2)) {
				// 自キャラの衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				playerBullet->OnCollision();
			}
		}
	}
	#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	//リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::EnemySet(Vector3 pos, Vector3 velocity) {
	// 敵を生成し、初期化
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(model_, pos, velocity);
	newEnemy->SetGameScene(this);

	// 敵を登録
	// gameScene_->enemyBullets_.push_back(newBullet);
	enemy_.push_back(newEnemy);
}

void GameScene::LoadEnemyPopData() {
	//ファイルを開く
	std::ifstream file;
	file.open("./Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {

	//待機処理
	if (waitFlag) {
		waitTimer--;
		if (waitTimer <= 0) {
			//待機完了
			waitFlag = false;
		}
		return;
	}

	//1行分の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			EnemySet({x, y, z}, {0.0f, 0.0f, -0.2f});

		}

		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			waitFlag = true;
			waitTimer = waitTime;

			//コマンドループを抜ける
			break;
		}
	}
}