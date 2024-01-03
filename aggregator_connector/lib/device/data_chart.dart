import 'dart:developer' as dev;

import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';

class DeviceChartContext extends ChangeNotifier {
  DeviceChartContext() : super() {
    dev.log("DeviceChartContext created");
  }

  void update(List<double> x, List<double> y) {
    // print(x);
    // print(y);
    // print(length);
    // this.x = [1, 2, 3, 4];
    // this.y = [1, 2, 3, 2];
    // length = 4;
    notifyListeners();
  }

  @override
  void dispose() {
    // TODO: implement dispose
    super.dispose();
    dev.log('DeviceChartContext disposed');
  }
}

class DeviceChart extends StatelessWidget {
  final List<double> y;
  final List<double> x;
  final double? yMin;
  final double? yMax;

  const DeviceChart(
      {required this.x, required this.y, this.yMin, this.yMax, super.key});

  @override
  Widget build(BuildContext context) {
    var points = <FlSpot>[];
    if (x.length != y.length) {
      throw Exception("x and y lists must be the same length to create chart.");
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
                dotData: FlDotData(
                  show: false,
                )),
          ],
          minY: yMin,
          maxY: yMax,
        ),
        swapAnimationDuration: const Duration(milliseconds: 1), // Optional
        swapAnimationCurve: Curves.linear, // Optional
      ),
    );
  }
}
