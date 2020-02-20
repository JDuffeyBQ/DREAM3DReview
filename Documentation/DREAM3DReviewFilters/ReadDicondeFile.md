# Read DICONDE File #

## Group (Subgroup) ##

DREAM3DReview (IO)

## Description ##

This filter will read a DICOM/DICONDE file and extract its image data into a DataArray of appropriate type and size. This filter creates a DataContainer with a 2D geometry for the image along with the appropriate AttributeMatrix.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Input File | Path | Input file path |
| Created Geometry | String | Created DataContainer name |

## Required Geometry ##

N/A

## Required Objects ##

N/A

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **DataContainer** | "DataContainer" | ImageGeometry | N/A | Contains the ImageGeometry of DICONDE file |
| **AttributeMatrix** | "CellData" | Cell | N/A | Contains the DataArray of image data |
| **DataArray** | "DicondeData" | uint8 / int8 / uint16 / int16 / uint32 / int32 | 1 | Image data |

## Example Pipelines ##

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
