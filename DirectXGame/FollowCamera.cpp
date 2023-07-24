#include "FollowCamera.h"
#include <MyMath.h>
#include <Input.h>

void FollowCamera::Initialize() {
	viewProjection_.Initialize();
}

void FollowCamera::Update() {
	//追従対象がいれば
	if (target_) {
		//追従対象からカメラへのオフセット
		Vector3 offset = {0.0f, 2.0f, -10.0f};

		Matrix4x4 rotateMatrix = MakeRotateYMatrix(viewProjection_.rotation_.y);

		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, rotateMatrix);

		//座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Vec3Addition(target_->translation_,  offset);
	}
	
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		// 速さ
		const float speed = 0.1f;

		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * speed;
	}

	viewProjection_.UpdateMatrix();
}