#include "Enemy.h"
#include <cassert>
#include <MyMath.h>
#include <GameScene.h>

Enemy::~Enemy() {
	
}

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("red.png");

	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	velocity_ = velocity;

	PhaseApproachInitialize();

}

void Enemy::Update() {

	switch (phase_) { 
	case Phase::Approach:
	default:
		PhaseApproach();
		break;
	case Phase::Leave:
		PhaseLeave();
		break;
	}

	Attack();

	// 行列更新
	worldTransform_.UpdateMatrix();

}

void Enemy::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Enemy::PhaseApproach() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, velocity_);
	// 既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}

	//発射タイマーカウントダウン
	FireTimer--;
	//指定時間に達した
	if (FireTimer <= 0) {
		//弾を発射
		Fire();
		//発射タイマーを初期化
		FireTimer = kFireInterval;
	}
}

void Enemy::PhaseLeave() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, leaveVelocity_);
}

void Enemy::Attack() { 

}

void Enemy::Fire() {
	assert(player_);
	
	// 弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(kBulletSpeed, kBulletSpeed, kBulletSpeed);

	Vector3 PlayerPos = playerPos_;
	Vector3 EnemyPos = GetWorldPosition();
	Vector3 Length = Vec3Subtraction(PlayerPos, EnemyPos);
	Vector3 NormalizeLength = Normalize(Length);
	velocity = Vec3Multiplication(NormalizeLength, velocity);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録
	//gameScene_->enemyBullets_.push_back(newBullet);
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::PhaseApproachInitialize() {
	//発射タイマーを初期化
	FireTimer = kFireInterval;
}

Vector3 Enemy::GetWorldPosition() {
	//ワールド座標を入れる変数
	Vector3 worldPos{};
	//ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

void Enemy::OnCollision() { 
	isDead_ = true;
}