/*
Team Members: Christian Hopkins, Quinn Carson
*/


#define _CRT_SECURE_NO_DEPRECATE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//in our model sample image 
//PPM header is : 
//P6
//800 800 //Number of columns(width) Number of rows (Height)
//255  //Max channel value
typedef struct {
	unsigned char red, green, blue;
} PPMPixel;

typedef struct {
	int x, y;
	PPMPixel* data;
} PPMImage;

#define RGB_COMPONENT_COLOR 255
const int WIDTH = 813;
const int HEIGHT = 708;

static PPMImage* readPPM(const char* filename)
{
	char buff[16];
	PPMImage* img;
	FILE* fp;
	int c, rgb_comp_color;
	//open PPM file for reading
	fp = fopen(filename, "rb");//reading a binary file
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

	//check the image format can be P3 or P6. P3:data is in ASCII format P6: data is in byte format
	if (buff[0] != 'P' || buff[1] != '6') {
		fprintf(stderr, "Invalid image format (must be 'P6')\n");
		exit(1);
	}

	//alloc memory form image
	img = (PPMImage*)malloc(sizeof(PPMImage));
	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//check for comments
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n');
		c = getc(fp);
	}

	ungetc(c, fp);//last character read was out back
	//read image size information
	if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {//if not reading widht and height of image means if there is no 2 values
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}

	//read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
		exit(1);
	}

	//check rgb component depth
	if (rgb_comp_color != RGB_COMPONENT_COLOR) {
		fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n');
	//memory allocation for pixel data for all pixels
	img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

	if (!img) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}

	//read pixel data from file
	if (fread(img->data, 3 * img->x, img->y, fp) != img->y) { //3 channels, RGB  //size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
		/*ptr − This is the pointer to a block of memory with a minimum size of size*nmemb bytes.
		  size − This is the size in bytes of each element to be read.
		  nmemb − This is the number of elements, each one with a size of size bytes.
		  stream − This is the pointer to a FILE object that specifies an input stream.
		*/
		fprintf(stderr, "Error loading image '%s'\n", filename);
		exit(1);
	}

	fclose(fp);
	return img;
}
void writePPM(const char* filename, PPMImage* img)
{
	FILE* fp;
	//open file for output
	fp = fopen(filename, "wb");//writing in binary format
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//write the header file
	//image format
	fprintf(fp, "P6\n");

	//image size
	fprintf(fp, "%d %d\n", img->x, img->y);

	// rgb component depth
	fprintf(fp, "%d\n", RGB_COMPONENT_COLOR);

	// pixel data
	fwrite(img->data, 3 * img->x, img->y, fp);
	fclose(fp);
}

void changeColorPPM(PPMImage* img)//Negating image
{
	int i;
	if (img) {

		for (i = 0; i < img->x * img->y; i++) {
			img->data[i].red = RGB_COMPONENT_COLOR - img->data[i].red;
			img->data[i].green = RGB_COMPONENT_COLOR - img->data[i].green;
			img->data[i].blue = RGB_COMPONENT_COLOR - img->data[i].blue;
		}
	}
}

void smoothImage(PPMImage* img) {
	int i;
	int pixel = 0;
	int redsum = 0;
	int greensum = 0;
	int bluesum = 0;


	if (img) {
		// advance through each pixel in image
		for (pixel; pixel < img->x * img->y; pixel++) {
			// check if top left corner pixel
			if (pixel == 0) {
				// blend red
				redsum += img->data[pixel + 1].red;
				redsum += img->data[pixel + WIDTH].red;
				redsum += img->data[pixel + WIDTH + 1].red;

				redsum /= 4;
				img->data[pixel].red = redsum;

				// blend green
				greensum += img->data[pixel + 1].green;
				greensum += img->data[pixel + WIDTH].green;
				greensum += img->data[pixel + WIDTH + 1].green;

				greensum /= 4;
				img->data[pixel].green = greensum;

				// blend blue
				bluesum += img->data[pixel + 1].blue;
				bluesum += img->data[pixel + WIDTH].blue;
				bluesum += img->data[pixel + WIDTH + 1].blue;

				bluesum /= 4;
				img->data[pixel].blue = bluesum;

			}

			// check if top right corner pixel
			else if (pixel == WIDTH - 1) {
				// blend red
				redsum += img->data[pixel - 1].red;
				redsum += img->data[pixel + WIDTH].red;
				redsum += img->data[pixel + WIDTH - 1].red;

				redsum /= 4;
				img->data[pixel].red = redsum;

				// blend green
				greensum += img->data[pixel - 1].green;
				greensum += img->data[pixel + WIDTH].green;
				greensum += img->data[pixel + WIDTH - 1].green;

				greensum /= 4;
				img->data[pixel].green = greensum;

				// blend blue
				bluesum += img->data[pixel - 1].blue;
				bluesum += img->data[pixel + WIDTH].blue;
				bluesum += img->data[pixel + WIDTH - 1].blue;

				bluesum /= 4;
				img->data[pixel].blue = bluesum;
			}

			// check for top border			
			else if (pixel < WIDTH) {
				// blend red
				redsum += img->data[pixel - 1].red;
				redsum += img->data[pixel + 1].red;

				redsum += img->data[pixel + WIDTH].red;
				redsum += img->data[pixel + WIDTH - 1].red;
				redsum += img->data[pixel + WIDTH + 1].red;

				redsum /= 6;
				img->data[pixel].red = redsum;

				// blend green
				greensum += img->data[pixel - 1].green;
				greensum += img->data[pixel + 1].green;

				greensum += img->data[pixel + WIDTH - 1].green;
				greensum += img->data[pixel + WIDTH + 1].green;

				greensum /= 6;
				img->data[pixel].green = greensum;

				// blend blue
				bluesum += img->data[pixel - 1].blue;
				bluesum += img->data[pixel + 1].blue;

				bluesum += img->data[pixel + WIDTH].blue;
				bluesum += img->data[pixel + WIDTH - 1].blue;
				bluesum += img->data[pixel + WIDTH + 1].blue;

				bluesum /= 6;
				img->data[pixel].blue = bluesum;
			}

			// main image
			else {
				// blend red
				redsum += img->data[pixel - 1].red;
				redsum += img->data[pixel + 1].red;

				redsum += img->data[pixel - WIDTH].red;
				redsum += img->data[pixel - WIDTH - 1].red;
				redsum += img->data[pixel - WIDTH + 1].red;

				redsum += img->data[pixel + WIDTH].red;
				redsum += img->data[pixel + WIDTH - 1].red;
				redsum += img->data[pixel + WIDTH + 1].red;

				redsum /= 9;

				img->data[pixel].red = redsum;

				// blend green
				greensum += img->data[pixel - 1].green;
				greensum += img->data[pixel + 1].green;

				greensum += img->data[pixel - WIDTH].green;
				greensum += img->data[pixel - WIDTH - 1].green;
				greensum += img->data[pixel - WIDTH + 1].green;

				greensum += img->data[pixel + WIDTH].green;
				greensum += img->data[pixel + WIDTH - 1].green;
				greensum += img->data[pixel + WIDTH + 1].green;

				greensum /= 9;
				img->data[pixel].green = greensum;

				// blend blue
				bluesum += img->data[pixel - 1].blue;
				bluesum += img->data[pixel + 1].blue;

				bluesum += img->data[pixel - WIDTH].blue;
				bluesum += img->data[pixel - WIDTH - 1].blue;
				bluesum += img->data[pixel - WIDTH + 1].blue;

				bluesum += img->data[pixel + WIDTH].blue;
				bluesum += img->data[pixel + WIDTH - 1].blue;
				bluesum += img->data[pixel + WIDTH + 1].blue;

				bluesum /= 9;
				img->data[pixel].blue = bluesum;
			}
		}


	}
	else {
		printf("Image did not load correctly.\n");
	}
}

void rotatePPM(PPMImage *img)//Rotating image
{
	/*
	Description:
	This code accepts a PPMImage pointer that represents a struct containing two ints that represent
	the size of the PPM image matrix. The code creates a temporary pointer to PPMPixel and a PPMPixel variable.
	The code loops through the array once to transpose the array and again to exchange the rows of the array.
	Resulting in an image rotated 90 degrees counter-clockwise

	Limitations:
	The current code uses double the size of the image in memory due to the temporary PPMPixel array.

*/

	if (img) {
		PPMPixel sTemp;
		PPMPixel *temp;
		temp = (PPMPixel *)malloc(img->x*img->y * sizeof(PPMPixel));
		memcpy(temp, img->data, img->x*img->y * sizeof(PPMPixel));

		/*
		//INTERMEDIATE/DERIVED VARIABLE NOTES
		width = img->x
		height = img->Y
		x = i % width
		y = i / width
		X + WIDTH * Y <-- index based on calculated x y
		Y + HEIGHT * X <-- transpose index derived from index
		//SKELETON LOOP
		//NOTE: MUST BE SQUARE MATRIX TO FUNCTION
		for (int i = 0; i < (WIDTH*HEIGHT); i++) {
			img->data[i] = temp[Y + HEIGHT * X];
		}
		*/
		//transposition loop
		for (int i = 0; i < (img->x*img->y); i++) {
			img->data[i] = temp[(i / img->x) + (img->y) * (i % img->x)];
		}

		/*
		//INTERMEDIATE/DERIVED VARIABLE NOTES
		width = img->x
		height = img->Y
		x = i % width
		y = (height - 1) - (i / width) <-- This represents the bottom row to be swapped
		X + WIDTH * Y <-- index based on derived x y

		//SKELETON LOOP
		//NOTE: MUST BE SQUARE MATRIX TO FUNCTION
		for (int i = 0; i < (WIDTH*HEIGHT)/2; i++) {
			sTemp = img->data[i];
			img->data[i] = img->data[x + width*y];
			img->data[x + width * y] = sTemp;
		}
		*/
		//Row Exchange
		for (int i = 0; i < (img->x*img->y) / 2; i++) {
			sTemp = img->data[i];
			img->data[i] = img->data[(i % img->x) + (img->x)*((img->y - 1) - (i / img->x))];
			img->data[(i % img->x) + (img->x) * ((img->y - 1) - (i / img->x))] = sTemp;
		}

		free(temp);
	}
}


int main() {
	PPMImage* image;
	PPMImage* image1;

	image = readPPM("model.ppm");
	//changeColorPPM(image);
	smoothImage(image);
	writePPM("model-2.ppm", image);

	//NOTE: model1 is a cropped square version of model.ppm
	image1 = readPPM("model1.ppm");
	rotatePPM(image1);
	writePPM("model-3.ppm", image1);
	printf("Press any key...");
	getchar();
}