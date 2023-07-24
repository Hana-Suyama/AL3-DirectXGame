#include "Player.h"
#include <cassert>
#include <Input.h>
#include <MyMath.h>

void Player::Initialize(Model * model, uint32_t textureHandle) {

	//ぬるぽチェック
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();

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

	worldTransform_.UpdateMatrix();

}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
