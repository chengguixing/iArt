#include <windows.h>
#include <stdio.h>

// 所有操作已假定是24位位图

// 图像数据区
// 4 * 4
BYTE myimage[12][12] = 
{
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
};

int BytesOneline(int width)
{
	return (width * 3) % 4 == 0 ? (width * 3): (((width * 3) / 4 + 1) * 4);
}

// create BMP
BOOL CreateBmp(int width, int height)
{
	BITMAPFILEHEADER  fileHeader;
	memset(&fileHeader, 0, sizeof(fileHeader));
	fileHeader.bfType = 'MB';
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BytesOneline(width) * height;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER  imageHeader;
	memset(&imageHeader, 0, sizeof(BITMAPINFOHEADER));
	imageHeader.biBitCount = 24;
	imageHeader.biHeight = height;
	imageHeader.biWidth = width;
	imageHeader.biPlanes = 1;
	imageHeader.biSize = sizeof(BITMAPINFOHEADER);

	FILE * file = fopen("c://bmp24.bmp", "wb");
	if (file)
	{
		fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), file);
		fwrite(&imageHeader, 1, sizeof(BITMAPINFOHEADER), file);

		for (int pos = 3; pos >= 0; --pos)  // 倒着写，正着看
		{
			fwrite(myimage + pos, 1, 12, file);
		}
		fclose(file);
	}

	return TRUE;
}

int main(void)
{
	CreateBmp(4, 4);

	return 0;
}