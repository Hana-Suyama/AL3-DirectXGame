#include "Ground.h"
#include <cassert>

void Ground::Initialize(Model* model) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0, 0, 0};

	// NULLポインタチェック
	assert(model);
	model_ = model;
}

void Ground::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Ground::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
