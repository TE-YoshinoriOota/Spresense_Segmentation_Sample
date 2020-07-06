# Spresense Segmentation Sample
Semantic Segmentation Sample Sketch for Spresense

<img src="https://github.com/TE-YoshinoriOota/Spresense_Segmentation_Sample/blob/master/resources/Spresense_Segmentation.png", width="600", height="450"/>


## Get Neural Network Console
You can get ![Sony's Neural Network Console](https://dl.sony.com) for free.

## Open the project
binary_semantic_segmentation_28_color.sdcproj is a neural network confole project for Semantic Segmentation. 

## Set the dataset
training_data/train.zip is the training data
training_data/valid.zip is the validation data
Extracting them and set them to dataset for the project

## Training and Evaluation
Training the network using Neural Network Console

## Copy model.nnb
After training, export model.nnb on a folder and copy it to SD card.

## Flashing Spresense_Segmentation_TestCam.ino to Spresense
Insert the SD card to Spresense Extention Board and flashing the sketch to Spresense.
If everything goes well, you can get the result like the pictuire above.
