#pragma once
#include <Model.h>
#include <WorldTransform.h>
#include <EnemyBullet.h>
#include <list>
    /// <summary>
/// 敵
/// </summary>
class Enemy {
public:
	/// <summary>
	/// デクストラクタ
	/// </summary>
	~Enemy();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	void PhaseApproach();
	void PhaseLeave();

	void PhaseApproachInitialize();

	static const int kFireInterval = 60;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 速度
	Vector3 velocity_;
	Vector3 leaveVelocity_ = {1, 1, 0};
	//発射タイマー
	int32_t FireTimer = 0;
	//行動フェーズ
	enum class Phase {
		Approach,	//接近する
		Leave,		//離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;

	// 弾
	std::list<EnemyBullet*> bullets_;

};
