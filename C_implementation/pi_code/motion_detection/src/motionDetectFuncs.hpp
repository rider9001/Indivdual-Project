#include <stdexcept>

ImgMtx * imageSubtract(ImgMtx * img1, ImgMtx * img2);
boundingBox detectMotion(ImgMtx *);

boundingBox HFTstDetectMotion(ImgMtx *);
bool sectorPass(ImgMtx *, int xOrigin, int yOrigin, const int sectorLen);

ImgMtx * MaskImg(ImgMtx *, boundingBox);