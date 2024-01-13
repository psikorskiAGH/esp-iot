import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';

class DeviceChart extends StatelessWidget {
  final List<double> y;
  final List<double> x;
  final double? yMin;
  final double? yMax;
  final String xLabel;
  final String yLabel;
  final String title;

  const DeviceChart({
    required this.x,
    required this.y,
    this.yMin,
    this.yMax,
    this.xLabel = 'Indexes',
    this.yLabel = 'Values',
    this.title = 'Title',
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    var points = <FlSpot>[];
    if (x.length != y.length) {
      // throw Exception("x and y lists must be the same length to create chart.");
      return Text(
        "[$title] Error: x and y lists must be the same length to create chart. ",
        style: const TextStyle(
          color: Color(0xFFCC0000),
        ),
      );
    }
    for (var i = 0; i < x.length; i++) {
      points.add(FlSpot(x[i], y[i]));
    }
    return ConstrainedBox(
      constraints: const BoxConstraints(maxHeight: 400),
      child: LineChart(
        LineChartData(
          lineBarsData: [
            LineChartBarData(
                spots: points,
                dotData: const FlDotData(
                  show: false,
                )),
          ],
          minY: yMin,
          maxY: yMax,
          titlesData: FlTitlesData(
            leftTitles: AxisTitles(
              axisNameWidget: Text(yLabel),
              sideTitles: const SideTitles(
                showTitles: true,
                reservedSize: 32,
                // getTitlesWidget: (value, meta) => Text("y-axis"),
              ),
              axisNameSize: 32,
            ),
            rightTitles: AxisTitles(
              axisNameWidget: Container(),
              axisNameSize: 32,
            ),
            bottomTitles: AxisTitles(
              axisNameWidget: Text(xLabel),
              sideTitles: const SideTitles(
                showTitles: true,
                reservedSize: 24,
                // getTitlesWidget: (value, meta) => Text("y-axis"),
              ),
              axisNameSize: 32,
            ),
            topTitles: AxisTitles(
              axisNameWidget: Text(title),
              axisNameSize: 32,
            ),
          ),
        ),
        duration: const Duration(milliseconds: 1), // Optional
        curve: Curves.linear, // Optional
      ),
    );
  }
}
