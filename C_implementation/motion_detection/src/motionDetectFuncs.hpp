#include <stdexcept>

ImgMtx * imageSubtract(ImgMtx * img1, ImgMtx * img2);
boundingBox detectMotion(ImgMtx *);
ImgMtx * MaskImg(ImgMtx *, boundingBox);