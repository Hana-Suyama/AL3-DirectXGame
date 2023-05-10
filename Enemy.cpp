﻿#include "Enemy.h"
#include <cassert>
#include <MyMath.h>

Enemy::~Enemy() {
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
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

	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

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
	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列更新
	worldTransform_.UpdateMatrix();

}

void Enemy::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
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
	// 弾の速度
	const float kBulletSpeed = -2.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録
	bullets_.push_back(newBullet);
}

void Enemy::PhaseApproachInitialize() {
	//発射タイマーを初期化
	FireTimer = kFireInterval;
}