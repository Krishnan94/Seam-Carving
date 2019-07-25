/* Name- Krishnan Krishnamoorthy
 Id- 40089054*/
#include "sc.h"

using namespace cv;
using namespace std;


bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }

    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }

    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;

    }


    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
   
  int differenceheight=in_image.cols-new_width;


   int differencewidth=in_image.rows-new_height;
     int i=0;
while(iimage.rows!=new_height || iimage.cols!=new_width){
        // horizontal seam if needed
       if(iimage.rows>new_height){          
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.cols>new_width){            
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    } 

    out_image = oimage.clone();
    return true;
}

Mat getGradient(Mat &in_image)
{
  Mat gradientSource,blurScource,graySource,gradientX,gradientY,absGradietntX,absGradietntY,gradient;
 // Reference - (Use the OpenCV function Sobel to calculate the derivatives from an input image) from https://docs.opencv.org/2.4.13.7/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html
  int s=0;
  int d =s+1;
 int ddepth = CV_16S;
    cvtColor(in_image, graySource, CV_BGR2GRAY);     
    Sobel(graySource, gradientX, CV_32F, d, s,3);
    Sobel(graySource, gradientY, CV_32F, s, d,3);
    convertScaleAbs(gradientX, absGradietntX);
    convertScaleAbs(gradientY, absGradietntY); 
    addWeighted( absGradietntX, 0.5, absGradietntY, 0.5, 0, gradient ); 
    absGradietntY.release();
    absGradietntX.release();
    gradientX.release();
    gradientY.release();
    graySource.release();
    return gradient;
}
void newUpdatedImage(Mat &in_image,Mat& newImage,vector<int> &dynamicPath,int direction)
{
 int oldRow=in_image.rows;
  int oldcolumn=in_image.cols;
 if(direction==0)
{
  
  for(int i=0;i<oldRow;i++)
  {
  
   for(int j=0;j<oldcolumn;j++)
  {
   if(j<dynamicPath[i]){
    newImage.at<Vec3b>(i,j)=in_image.at<Vec3b>(i,j);
  
    }
   if(j>dynamicPath[i]){
     newImage.at<Vec3b>(i,j-1)=in_image.at<Vec3b>(i,j);
    }
   }
  
  }
}
else
{
 int newRow=0;
 for(int i=0;i<oldcolumn;i++)
  {
   for(int j=0;j<oldRow;j++)
  {
   if(j<dynamicPath[i]){
    newImage.at<Vec3b>(j,i)=in_image.at<Vec3b>(j,i);    
    }
 if(j>dynamicPath[i]){
  newImage.at<Vec3b>(j-1,i)=in_image.at<Vec3b>(j,i);
  }
   }  
  }
}
}
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){
Mat gradientSource=getGradient(in_image);
    int gradientRows = gradientSource.rows;
    int gradientColumns = gradientSource.cols;
    Mat newImage = Mat(in_image.rows-1, in_image.cols, CV_8UC3);
     int energySource[gradientRows][gradientColumns];

     for(int j = 0; j < gradientColumns; j++){
    for(int i = 0; i < gradientRows; i++){       
         if(j==0)
           {
                energySource[i][j]= (int)gradientSource.at<uchar>(i, j);
           }
           else
           {       
           int left = energySource[max(i - 1,0)][ j - 1];
           int up= energySource[i][j-1];
           int  right = energySource[min(i + 1,gradientRows-1)][j - 1];                
          energySource[i][j]= (int)gradientSource.at<uchar>(i, j) + min(left, min(up,right));
          }
        }
    }
int minColValue= energySource[0][gradientColumns-1];
   int minRowValue= 0;
   vector<int> dynamicPath(gradientColumns);   
    for(int j=1;j<gradientRows;j++){
        if(minColValue > energySource[j][gradientColumns-1]){
            minColValue = energySource[j][gradientColumns-1];
            minRowValue = j;
        }
    }
  dynamicPath[gradientColumns-1]=minRowValue; 
   for(int i = gradientColumns-1;i>0;i--)  
   {
       int left = energySource[max(minRowValue - 1,0)][i-1];      
       int  right = energySource[min(minRowValue + 1, gradientRows - 1)][i-1];           
            if((energySource[minRowValue][i] - (int)gradientSource.at<uchar>(minRowValue,i)) == left)
                minRowValue=minRowValue-1;
            else if((energySource[minRowValue][i] - (int)gradientSource.at<uchar>(minRowValue,i)) == right)
                minRowValue=minRowValue+1;
           minRowValue = min(max(minRowValue, 0), gradientRows - 1);      
        dynamicPath[i]=minRowValue;
    }
   newUpdatedImage(in_image,newImage,dynamicPath,1);
    
    out_image=newImage.clone();
  
    newImage.release();

return true;
}
bool reduce_vertical_seam_trivial(Mat& in_image,Mat& out_image)
 {
  Mat gradientSource=getGradient(in_image);
    int gradientRows = gradientSource.rows;
    int gradientColumns = gradientSource.cols;
    Mat newImage = Mat(in_image.rows, in_image.cols-1, CV_8UC3);

    int energySource[gradientRows][gradientColumns];

    
    for(int i = 0; i < gradientRows; i++){
        for(int j = 0; j < gradientColumns; j++){
         if(i==0)
           {
                energySource[i][j]= (int)gradientSource.at<uchar>(i, j);
           }
           else
           {       
           int left = energySource[i - 1][ max(j - 1, 0)];
           int up= energySource[i - 1][j];
           int  right = energySource[i - 1][min(j + 1, gradientColumns - 1)];                
          energySource[i][j]= (int)gradientSource.at<uchar>(i, j) + min(left, min(up,right));
          }
        }
    }
    int minRowValue= energySource[gradientRows-1][0];
    int minColumValue= 0;
   vector<int> dynamicPath(gradientRows);
   

    for(int j=1;j<gradientColumns;j++){
        if(minRowValue > energySource[gradientRows-1][j]){
            minRowValue = energySource[gradientRows-1][j];
            minColumValue = j;
        }
    }  
   
    dynamicPath[gradientRows-1]=minColumValue; 
   for(int i = gradientRows-1;i>0;i--)  
   {
       int left = energySource[i - 1][ max(minColumValue - 1, 0)];      
       int  right = energySource[i - 1][min(minColumValue + 1, gradientColumns - 1)];           
            if((energySource[i][minColumValue] - (int)gradientSource.at<uchar>(i,minColumValue)) == left)
                minColumValue=minColumValue-1;
            else if((energySource[i][minColumValue] - (int)gradientSource.at<uchar>(i,minColumValue)) == right)
                minColumValue=minColumValue+1;
           minColumValue = min(max(minColumValue, 0), gradientColumns - 1);      
        dynamicPath[i]=minColumValue;
    }
    newUpdatedImage(in_image,newImage,dynamicPath,0);    
    out_image=newImage.clone();  
    newImage.release();
    return true;
}





