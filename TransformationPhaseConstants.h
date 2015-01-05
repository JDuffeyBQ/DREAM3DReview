#ifndef _TransformationPhaseConstants_H_
#define _TransformationPhaseConstants_H_

#include <QtCore/QString>

namespace TransformationPhase
{
  const QString TransformationPhasePluginFile("TransformationPhasePlugin");
  const QString TransformationPhasePluginDisplayName("TransformationPhasePlugin");
  const QString TransformationPhaseBaseName("TransformationPhasePlugin");

  const DREAM3D_STRING Initiators("Initiators");
  const DREAM3D_STRING HardFeatures("HardFeatures");
  const DREAM3D_STRING SoftFeatures("SoftFeatures");
  const DREAM3D_STRING HardSoftGroups("HardSoftGroups");
  const DREAM3D_STRING SelectedFeatures("SelectedFeatures");
  const DREAM3D_STRING SubsurfaceFeatures("SubsurfaceFeatures");

  const DREAM3D_STRING SurfaceMeshCSLBoundary("SurfaceMeshCSLBoundary");
  const DREAM3D_STRING SurfaceMeshCSLBoundaryIncoherence("SurfaceMeshCSLBoundaryIncoherence");
  namespace CSL
  {

    const float Sigma3 = 3.0f;
    const float Sigma5 = 5.0f;
    const float Sigma7 = 7.0f;
    const float Sigma9 = 9.0f;
    const float Sigma11a = 11.0f;
    const float Sigma11b = 11.5f;
    const float Sigma13a = 13.0f;
    const float Sigma13b = 13.5f;
    const float Sigma15 = 15.0f;
    const float Sigma17a = 17.0f;
    const float Sigma17b = 17.5f;
    const float Sigma19a = 19.0f;
    const float Sigma19b = 19.5f;
    const float Sigma21a = 21.0f;
    const float Sigma21b = 21.5f;
    const float Sigma23 = 23.0f;
    const float Sigma25a = 25.0f;
    const float Sigma25b = 25.5f;
    const float Sigma27a = 27.0f;
    const float Sigma27b = 27.5f;
    const float Sigma29a = 29.0f;
    const float Sigma29b = 29.5f;

  }

  static const float CSLAxisAngle[21][5] =
  {
	{3.0f, 60.0f, 1.0f, 1.0f, 1.0f},
	{5.0f, 36.87f, 1.0f, 0.0f, 0.0f},
	{7.0f, 38.21f, 1.0f, 1.0f, 0.0f},
	{9.0f, 38.94f, 1.0f, 1.0f, 0.0f},
	{11.0f, 50.48f, 1.0f, 1.0f, 0.0f},
	{13.0f, 22.62f, 1.0f, 0.0f, 0.0f},
	{13.5f, 27.8f, 1.0f, 1.0f, 1.0f},
	{15.0f, 48.19f, 2.0f, 1.0f, 0.0f},
	{17.0f, 28.07f, 1.0f, 0.0f, 0.0f},
	{17.5f, 61.93f, 2.0f, 2.0f, 1.0f},
	{19.0f, 26.53f, 1.0f, 1.0f, 0.0f},
	{19.5f, 46.83f, 1.0f, 1.0f, 1.0f},
	{21.0f, 21.79f, 1.0f, 1.0f, 1.0f},
	{21.5f, 44.4f, 2.0f, 1.0f, 1.0f},
	{23.0f, 40.45f, 3.0f, 1.0f, 1.0f},
	{25.0f, 16.25f, 1.0f, 0.0f, 0.0f},
	{25.5f, 51.68f, 3.0f, 3.0f, 1.0f},
	{27.0f, 31.58f, 1.0f, 1.0f, 0.0f},
	{27.5f, 35.42f, 2.0f, 1.0f, 0.0f},
	{29.0f, 43.61f, 1.0f, 0.0f, 0.0f},
	{29.5f, 46.39f, 2.0f, 2.0f, 1.0f}
  };
}
#endif