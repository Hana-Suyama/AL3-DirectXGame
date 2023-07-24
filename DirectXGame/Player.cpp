#include "Player.h"
#include <cassert>
#include <Input.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <MyMath.h>
#include <imgui.h>

void Player::Initialize(Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm) {

	//ぬるぽチェック
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);

	modelBody_ = modelBody;
	modelHead_ = modelHead;
	modelL_arm_ = modelL_arm;
	modelR_arm_ = modelR_arm;

	//textureHandle_ = textureHandle;
	
	worldTransform_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	//親子関係
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;

	InitializeFloatingGimmick();


}

void Player::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		//速さ
		const float speed = 0.3f;

		//移動量
		Vector3 move = {(float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f, (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		//移動量に速さを反映
		move = Vec3FloatMultiplication(Normalize(move), speed);

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);

		move = TransformNormal(move, rotateMatrix);

		//移動
		worldTransform_.translation_ = Vec3Addition(worldTransform_.translation_, move);

		worldTransform_.rotation_.y = std::atan2(move.x, move.z);

	}
	
	UpdateFloatingGimmick();

	worldTransform_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

}

void Player::Draw(ViewProjection& viewProjection) {
	modelBody_->Draw(worldTransformBody_, viewProjection);
	modelHead_->Draw(worldTransformHead_, viewProjection);
	modelL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelR_arm_->Draw(worldTransformR_arm_, viewProjection);
}

void Player::InitializeFloatingGimmick() {
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() {
	//浮遊移動のサイクル<frame>
	uint16_t period = 120;
	//1フレームでのパラメータ加算値
	float step = (float)(2.0f * M_PI / period);

	//パラメータを1ステップ分加算
	floatingParameter_ += step;
	//2πを超えたら0に戻す
	floatingParameter_ = (float)(std::fmod(floatingParameter_, 2.0f * M_PI));

	//浮遊の振幅
	const float floatingAmplitude = 0.5f;
	//浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;

	UpdateFloatingArmGimmick();

	/*ImGui::Begin("Player");
	ImGui::SliderFloat3("Head Translation", reinterpret_cast<float *>(&worldTransformHead_.translation_), 0, 10);
	ImGui::SliderFloat3("ArmL Translation", reinterpret_cast<float *>(&worldTransformL_arm_.translation_), 0, 10);
	ImGui::SliderFloat3("ArmR Translation", reinterpret_cast<float *>(&worldTransformR_arm_.translation_), 0, 10);
	ImGui::SliderInt("period", reinterpret_cast<int *>(&period), 0, 10);
	ImGui::SliderFloat("step", &step, 0, 10);
	ImGui::End();*/
	
}

void Player::UpdateFloatingArmGimmick() {
	// 浮遊移動のサイクル<frame>
	uint16_t period = 120;
	// 1フレームでのパラメータ加算値
	float step = (float)(2.0f * M_PI / period);

	// パラメータを1ステップ分加算
	floatingArmParameter_ += step;
	// 2πを超えたら0に戻す
	floatingArmParameter_ = (float)(std::fmod(floatingArmParameter_, 2.0f * M_PI));

	// 浮遊の振幅
	const float floatingAmplitude = 0.5f;
	// 浮遊を座標に反映
	worldTransformL_arm_.rotation_.x = std::sin(floatingArmParameter_) * floatingAmplitude;
	worldTransformR_arm_.rotation_.x = std::sin(floatingArmParameter_) * floatingAmplitude;
}