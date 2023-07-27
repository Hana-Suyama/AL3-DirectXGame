#pragma once
#include <WorldTransform.h>
#include <Model.h>
#include <BaseCharacter.h>
#include <optional>

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

	//通常行動初期化
	void BehaviorRootInitialize();
	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動更新
	void BehaviorAttackUpdate();

	//調整項目の適用
	void ApplyGlobalVariables();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformHummer_;

	//モデル
	Model* model_ = nullptr;

	Model* modelBody_ = nullptr;
	Model* modelHead_ = nullptr;
	Model* modelL_arm_ = nullptr;
	Model* modelR_arm_ = nullptr;
	Model* modelHummer_ = nullptr;

	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊ギミック腕の媒介変数
	float floatingArmParameter_ = 0.0f;

	//攻撃アニメーション用
	float attackAnimParameter_ = 0.0f;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//振る舞い
	enum class Behavior {
		kRoot,	//通常攻撃
		kAttack,//攻撃中
	};

	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
};
