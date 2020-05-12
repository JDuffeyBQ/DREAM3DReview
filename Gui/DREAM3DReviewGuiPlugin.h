#pragma once

#include "DREAM3DReview/DREAM3DReviewPlugin.h"

class DREAM3DReviewGuiPlugin : public DREAM3DReviewPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "net.bluequartz.dream3d.DREAM3DReviewGuiPlugin")

public:
  DREAM3DReviewGuiPlugin();
  ~DREAM3DReviewGuiPlugin() override;

  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;

public:
  DREAM3DReviewGuiPlugin(const DREAM3DReviewGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  DREAM3DReviewGuiPlugin(DREAM3DReviewGuiPlugin&&) = delete;                 // Move Constructor Not Implemented
  DREAM3DReviewGuiPlugin& operator=(const DREAM3DReviewGuiPlugin&) = delete; // Copy Assignment Not Implemented
  DREAM3DReviewGuiPlugin& operator=(DREAM3DReviewGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
