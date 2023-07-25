#include "Enemy.h"
#include <cassert>
#include <MyMath.h>

void Enemy::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// ぬるぽチェック
	assert(models[0]);
	assert(models[1]);
	assert(models[2]);
	assert(models[3]);

	modelBody_ = models[0];
	modelHead_ = models[1];
	modelL_arm_ = models[2];
	modelR_arm_ = models[3];

	// textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	// 親子関係
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;

}

void Enemy::Update() {

	// 速さ
	const float speed = 0.3f;
	const float rotate = 0.02f;

	// 移動量
	Vector3 move = {0.0f, 0.0f, 1.0f};
	move = TransformNormal(move, worldTransform_.matWorld_);
	// 移動量に速さを反映
	move = Vec3FloatMultiplication(Normalize(move), speed);

	// 移動
	worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, move);

	worldTransform_.rotation_.y += rotate;

	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

}

void Enemy::Draw(ViewProjection& viewProjection) {
	models_[0]->Draw(worldTransformBody_, viewProjection);
	models_[1]->Draw(worldTransformHead_, viewProjection);
	models_[2]->Draw(worldTransformL_arm_, viewProjection);
	models_[3]->Draw(worldTransformR_arm_, viewProjection);
}