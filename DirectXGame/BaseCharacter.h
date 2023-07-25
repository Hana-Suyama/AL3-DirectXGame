#pragma once
#include <vector>
#include <Model.h>
class BaseCharacter {
protected:
	//モデルデータ配列
	std::vector<Model*> models_;
	//ワールド変換データ
	WorldTransform worldTransform_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models_">モデルデータ配列</param>
	virtual void Initialize(const std::vector<Model*>& models);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name ="viewProjection">ビュープロジェクション(参照渡し) </param>
	virtual void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <param name ="viewProjection">ワールド変換データ </param>
	const WorldTransform& GetWorldTransform(){return worldTransform_;};
	
};
