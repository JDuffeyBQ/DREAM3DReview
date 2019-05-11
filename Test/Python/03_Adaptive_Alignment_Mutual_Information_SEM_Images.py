"""

Pipeline example based on 03_Adaptive Alignment - Mutual Information - SEM Images in Anisotropy examples

"""

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy
from dream3d import dream3dreviewpy
from dream3d import itkimageprocessing
from dream3d import itkimageprocessingpy

# Numpy for some data management assistance
import numpy as np
import datetime as datetime


def adaptive_alignment():
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()

    # Register the ImageIO Factories
    imageWriter = itkimageprocessing.ITKImageWriter.New()    
    imageWriter.registerImageIOFactories()

    # Read H5EBSD File
    print("Loading H5EBSD File")
    err = orientationanalysispy.read_h5_ebsd(dca, "AlMgSc Data", "Phase Data", "EBSD SEM Scan Data",
                                            sd.GetBuildDirectory() + "/Data/Anisotropy/AlMgSc.h5ebsd",
                                            0, 9, True, sc.AngleRepresentation.Radians,
                                            simpl.StringSet({"Fit", "Image Quality", "EulerAngles",
                                                             "SEM Signal", "Confidence Index", "Phases"}))
    if err < 0:
        print("ReadH5Ebsd ErrorCondition %d" % err)
    
    # Import Image Stack [ITK]
    print("Loading Images...")
    fileListInfo = simpl.FileListInfo(3, 0, 0, 9, 1, sd.GetBuildDirectory() + "/Data/Anisotropy/tif",
                                      "AlMgSc-TD_", "", "tif")
    err = itkimageprocessingpy.itk_import_image_stack(dca, "SEMAlMgSc Data", "EBSD SEM Scan Data",
                                                      simpl.FloatVec3Type([0, 0, 0]), simpl.FloatVec3Type([1, 1, 1]),
                                                      fileListInfo, 10, "ImageData")
    if err < 0:
        print("ITK Import Image Stack ErrorCondition %d" % err)

    # Convert Orientation Representation
    print("Creating Quats....")
    err = orientationanalysispy.convert_orientations(dca, 0, 2,
                                                    simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "EulerAngles"),
                                                    "Quats")
    if err < 0:
        print("Convert Orientations ErrorCondition %d" % err)

    # Adaptive Alignment Mutual Information
    err = dream3dreviewpy.adaptive_alignment_mutual_information(dca, 5, False,
                                                             simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Quats"),
                                                             simpl.DataArrayPath("AlMgSc Data", "EBSD SEM Scan Data", "Phases"),
                                                             simpl.DataArrayPath("", "", ""),
                                                             simpl.DataArrayPath("AlMgSc Data", "Phase Data", "CrystalStructures"),
                                                             1,  # Global Correction
                                                             simpl.DataArrayPath("SEMAlMgSc Data", "EBSD SEM Scan Data", "ImageData"),
                                                             )
    if err < 0:
        print("AdaptiveAlignment Mutual Information %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    adaptive_alignment()


