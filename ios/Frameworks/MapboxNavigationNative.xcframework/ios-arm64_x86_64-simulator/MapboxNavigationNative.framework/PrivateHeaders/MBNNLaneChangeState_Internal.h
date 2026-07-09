// This file is generated and will be overwritten automatically.

#import <Foundation/Foundation.h>

// NOLINTNEXTLINE(modernize-use-using)
typedef NS_ENUM(NSInteger, MBNNLaneChangeState)
{
    /** Default state, the LaneChangeAssist component is not visible. */
    MBNNLaneChangeStateInactive,
    /**
     * The vehicle is planning to maneuver to the adjacent lane.
     * If LaneChangeAssistTheme#targetIndicatorPlanningColor is set,
     * this color will be used to style the model.
     */
    MBNNLaneChangeStatePlanning,
    /**
     * The vehicle is performing the maneuver.
     * If LaneChangeAssistTheme#targetIndicatorInProgressColor is set,
     * this color will be used to style the model.
     */
    MBNNLaneChangeStateInProgress
} NS_SWIFT_NAME(LaneChangeState);

NSString* MBNNLaneChangeStateToString(MBNNLaneChangeState lane_change_state);
