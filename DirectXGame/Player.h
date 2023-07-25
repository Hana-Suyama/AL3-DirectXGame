﻿#pragma once
#include <WorldTransform.h>
#include <Model.h>
#include <BaseCharacter.h>

class Player : public BaseCharacter {
public:

	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(ViewProjection& viewProjection) override;

	const WorldTransform& GetWorldTransform() { return worldTransform_; };

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	//浮遊ギミック初期化
	void InitializeFloatingGimmick();

	//浮遊ギミック更新
	void UpdateFloatingGimmick();

	// 浮遊ギミック腕更新
	void UpdateFloatingArmGimmick();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	//モデル
	Model* model_ = nullptr;

	Model* modelBody_ = nullptr;
	Model* modelHead_ = nullptr;
	Model* modelL_arm_ = nullptr;
	Model* modelR_arm_ = nullptr;

	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊ギミック腕の媒介変数
	float floatingArmParameter_ = 0.0f;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
};
