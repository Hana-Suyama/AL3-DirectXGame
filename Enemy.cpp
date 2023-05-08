#include "Enemy.h"
#include <cassert>
#include <MyMath.h>

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
}

void Enemy::PhaseLeave() {
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, leaveVelocity_);
}