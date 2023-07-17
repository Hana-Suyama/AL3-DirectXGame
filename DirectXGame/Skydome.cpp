#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Model* model) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0, 0, 0};

	// NULLポインタチェック
	assert(model);
	model_ = model;
}

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}