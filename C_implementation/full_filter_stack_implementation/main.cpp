#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "filterStages.h"
#include "boundingBoxStructs.h"

#include "ImgMtx.cpp"
#include "boxFilter.cpp"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		cout << "usage: filename" << endl;
		return 1;
	}

	const char * filename = argv[1];

	//test read-write, delete for speed
	/*
	int x,y,n;
	unsigned char * tstData = stbi_load(filename, &x, &y, &n, 0);
	stbi_write_jpg("TestOutput.jpg", x, y, n, tstData, 90);
	stbi_image_free(tstData);
	*/

	cout << "----Gray scale image----" << endl;

	auto start = std::chrono::system_clock::now();
    ImgMtx testImg(filename);
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	cout << "File " << filename << " read complete in " << elapsed_seconds.count() << "s" << endl;

	cout << "Image is: " << testImg.getWidth() << "x" << testImg.getHeight() << endl;

	const char * outFileName = "ImgOut/GrayscaleOutput.jpg";
	start = std::chrono::system_clock::now();

	cout << "Writing file: " << outFileName << endl;
	int code = testImg.writeImg(outFileName);
	end = std::chrono::system_clock::now();

	elapsed_seconds = end-start;

	cout << "File write complete in " << elapsed_seconds.count() << "s" << endl;

	if(code != 0)
	{
		cout << "File write success" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
	}

	cout << "----Gaussian filter----" << endl;

    cout << "Starting gaussian filter" << endl;
	start = std::chrono::system_clock::now();
    testImg.gaussBlur();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Gaussian filter complete in " << elapsed_seconds.count() << "s" << endl;
    outFileName = "ImgOut/GaussOutput.jpg";
    cout << "Writing file: " << outFileName << endl;
    code = testImg.writeImg(outFileName);
    if(code != 0)
	{
		cout << "File write success" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
		return code;
	}

    cout << "----Sobel filter----" << endl;

    cout << "Starting sobel filter." << endl;
    start = std::chrono::system_clock::now();
    testImg.SobelFil();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Sobel filter complete in " << elapsed_seconds.count() << "s" << endl;
    outFileName = "ImgOut/SobelOutput.jpg";
    cout << "Writing file: " << outFileName << endl;
    code = testImg.writeImg(outFileName);
    if(code != 0)
	{
		cout << "File write success" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
		return code;
	}

	cout << "----Non-max suppression----" << endl;

	cout << "Starting non-max suppression." << endl;
	start = std::chrono::system_clock::now();
    testImg.nonMaxSupress();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Non-max suppression complete in " << elapsed_seconds.count() << "s" << endl;
    outFileName = "ImgOut/SuppressionOutput.jpg";
    cout << "Writing file: " << outFileName << endl;
    code = testImg.writeImg(outFileName);
    if(code != 0)
	{
		cout << "File write success" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
		return code;
	}

	cout << "----Edge linking----" << endl;

    cout << "Starting edge linking." << endl;
	start = std::chrono::system_clock::now();
    testImg.edgeLink();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Edge linking complete in " << elapsed_seconds.count() << "s" << endl;
    outFileName = "ImgOut/edgeLinkedOutput.jpg";
    cout << "Writing file: " << outFileName << endl;
    code = testImg.writeImg(outFileName);
    if(code != 0)
	{
		cout << "File write success" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
		return code;
	}

	cout << "----Bounding boxes----" << endl;

	cout << "Starting bounding box finding." << endl;
	start = std::chrono::system_clock::now();
    vector<boundingBox> boxes = testImg.getBoundingBoxes();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Bounding box finding complete in " << elapsed_seconds.count() << "s" << endl;
    cout << boxes.size() << " boxes found." << endl;

    cout << endl;
    cout << "Starting box filtering." << endl;
    start = std::chrono::system_clock::now();
    vector<boundingBox> filtered = boxFilter(boxes, testImg.getWidth(), testImg.getHeight());
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << filtered.size() << " boxes filtered in " << elapsed_seconds.count() << "s" << endl;

    cout << "----Filtered box stats----" << endl;

    boundingBox avgBox;
    avgBox.x1 = 0;
    avgBox.y1 = 0;
    avgBox.x2 = 0;
    avgBox.y2 = 0;

    if(filtered.size() == 0)
    {
        cout << "No passing boxes found." << endl;
    }
    else
    {
         cout << "(x1,y1) -> (x2,y2)" << endl;
        for(unsigned int i = 0; i < filtered.size(); i++)
        {
            boundingBox curBox = filtered.at(i);
            cout << "Box " << (i+1) << ": (" << curBox.x1 << "," << curBox.y1 << ") -> (" << curBox.x2 << "," << curBox.y2 << "), Area: " << (curBox.y2 - curBox.y1) * (curBox.x2 - curBox.x1) <<endl;

            avgBox.x1 += curBox.x1 / filtered.size();
            avgBox.x2 += curBox.x2 / filtered.size();
            avgBox.y1 += curBox.y1 / filtered.size();
            avgBox.y2 += curBox.y2 / filtered.size();
        }

        cout << "Average area: " << (avgBox.y2 - avgBox.y1) * (avgBox.x2 - avgBox.x1) << endl;
        cout << "Average width: " << avgBox.x2 - avgBox.x1 << endl;
        cout << "Average height: " << avgBox.y2 - avgBox.y1 << endl;
        cout << "Average box: (" << avgBox.x1 << "," << avgBox.y1 << ") -> (" << avgBox.x2 << "," << avgBox.y2 << ")" << endl;
    }

    cout << "----All tests complete----" << endl;

	return 0;
}
