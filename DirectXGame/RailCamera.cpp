#include "RailCamera.h"
#include <cassert>
#include <MyMath.h>
#include "ImGuiManager.h"

void RailCamera::Initialize(Vector3 pos, Vector3 rad) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = rad;
	viewProjection_.farZ = 300;
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void RailCamera::Update() { 
	worldTransform_.translation_.z -= 0.1f;
	// 行列更新
	worldTransform_.UpdateMatrix();
	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	//カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::Text("translation %f.%f.%f", worldTransform_.translation_.x, worldTransform_.translation_.y,worldTransform_.translation_.z);
	ImGui::Text("rotation %f.%f.%f", worldTransform_.rotation_.x, worldTransform_.rotation_.y,worldTransform_.rotation_.z);
	ImGui::End();
}

