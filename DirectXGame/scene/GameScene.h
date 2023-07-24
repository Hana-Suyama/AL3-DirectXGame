﻿#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Player.h>
#include <memory>
#include <Skydome.h>
#include <DebugCamera.h>
#include <Ground.h>
#include <FollowCamera.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//天球
	std::unique_ptr<Skydome> skydome_;

	//地面
	std::unique_ptr<Ground> ground_;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデル
	std::unique_ptr<Model> model_ = nullptr;

	//天球3Dモデル
	std::unique_ptr<Model> modelSkydome_ = nullptr;

	// 地面3Dモデル
	std::unique_ptr<Model> modelGround_ = nullptr;

	//ビュープロジェクション
	ViewProjection viewProjection_;

	//自キャラ
	std::unique_ptr<Player> player_;

	bool isDebugCameraActive_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
