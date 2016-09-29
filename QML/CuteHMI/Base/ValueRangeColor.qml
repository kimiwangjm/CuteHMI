import QtQuick 2.0

//<workaround id="QML_Base-4" target="Qt" cause="QTBUG-34418">
// Singletons require explicit import to load qmldir file.
import "."
//</workaround>

import "ValueRangeColor.js" as Private

ValueColor
{
	property var ranges: [[-Infinity, Infinity]]
	property var colors: [implicitColor]
	property color nanColor: implicitColor

	function setStandardRanges(obj, palette)
	{
		ranges = []
		colors = []
		var lowerBound = -Infinity
		if ((typeof palette === "undefined"))
			palette = Palette
		if ((typeof obj.alertLow !== "undefined")) {
			ranges.push([lowerBound, obj.alertLow])
			colors.push(palette.alert)
			lowerBound = obj.alertLow
		}
		if ((typeof obj.warningLow !== "undefined")) {
			ranges.push([lowerBound, obj.warningLow])
			colors.push(palette.warning)
			lowerBound = obj.warningLow
		}
//		if ((typeof obj.low !== "undefined")) {
//			ranges.push([lowerBound, obj.low])
//			lowerBound = obj.low
//		}
		var upperBound = Infinity
		if ((typeof obj.alertHigh !== "undefined")) {
			ranges.push([obj.alertHigh, upperBound])
			colors.push(palette.alert)
			upperBound = obj.alertHigh
		}
		if ((typeof obj.warningHigh !== "undefined")) {
			ranges.push([obj.warningHigh, upperBound])
			colors.push(palette.warning)
			upperBound = obj.warningHigh
		}
//		if ((typeof obj.high !== "undefined")) {
//			ranges.push([obj.high, upperBound])
//			upperBound = obj.high
//		}
		ranges.push([lowerBound, upperBound])
		colors.push(implicitColor)

		Private.pickColor()
	}

	onValueChanged: Private.pickColor()

	onColorChanged: {
		if (colors[Private.currentRange] !== color)
			colors[Private.currentRange] = color
	}
}
