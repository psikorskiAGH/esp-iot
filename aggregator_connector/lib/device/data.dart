import 'dart:async';
import 'dart:developer' as dev;

import 'package:aggregator_connector/device/data_field.dart';
import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';

import '../http_path.dart';
import 'data_chart.dart';

const Duration graphRefreshInterval = Duration(milliseconds: 1000);

class DeviceDataContext extends ChangeNotifier {
  final JsonApi api;
  List<Map<String, dynamic>> data = [
    {"type": "single", "name": "Loading...", "value": "", "unit": ""}
  ];
  int _updating = 0;

  DeviceDataContext({required this.api});
  void update() async {
    if (_updating > 0) {
      dev.log("WARN: DeviceDataContext already updating.");
      return;
    }
    ++_updating;
    print("Updating");

    try {
      final HttpJsonResponse resp;
      final List<dynamic> respData;
      try {
        resp = await api.get();
        respData = resp.asList();
        data.clear();
        for (final r in respData) {
          data.add(r as Map<String, dynamic>);
        }
        // data.addAll(respData as List<Map<String, dynamic>>);
      } catch (e) {
        data.clear();
        data.add({
          "type": "single",
          "name": "Error",
          "value": e.toString(),
          "unit": ""
        });
      }
      notifyListeners();
    } finally {
      --_updating;
    }
  }
}

class DeviceData extends StatelessWidget {
  final DeviceDataContext ctx;
  late final Timer timer;
  final Map<String, DeviceChart> chartCache = {};

  DeviceData({required this.ctx, super.key}) {
    timer = Timer.periodic(graphRefreshInterval, (timer) {
      ctx.update();
    });
  }

  @override
  Widget build(BuildContext context) {
    final ret = Padding(
        padding: const EdgeInsets.all(16.0),
        child: ChangeNotifierProvider<DeviceDataContext>(
          create: (_) => ctx,
          child: Consumer<DeviceDataContext>(builder: tableBuilder),
        ));
    ctx.update();
    return ret;
  }

  Widget tableBuilder(
      BuildContext context, DeviceDataContext ctx, Widget? child) {
    var ch = <Widget>[];
    for (var el in ctx.data) {
      switch (el["type"]) {
        case "single":
          ch.add(DeviceDataValueRow(data: el));
          break;
        case "array":
        case "timearray":
          ch.add(_buildArrayDataRow(el));
          break;
        default:
          ch.add(DeviceDataErrorRow(
              description: "Unknown field type: '${el["type"]}'"));
      }
    }
    return Column(
      children: ch,
    );
  }

  Widget _buildArrayDataRow(Map<String, dynamic> data) {
    List<double> x;
    List<double> y = _parseValues(data["values"]);
    var timestamps = data["timestamps"];
    if (timestamps != null) {
      final v = _parseValues(timestamps).map((e) => e / 1000);
      final offset = v.first;
      x = List<double>.from(v.map((e) => e - offset)); // in [s]
    } else {
      x = List<double>.generate(y.length, (i) => i + 1);
    }

    String name = data["name"];
    DeviceChart chart = DeviceChart(
      x: x,
      y: y,
      yMin: data["min"]?.toDouble(),
      yMax: data["max"]?.toDouble(),
    );

    return Column(
      children: [
        Text(name),
        chart,
      ],
    );
  }

  List<double> _parseValues(List<dynamic> valuesData) {
    List<double> out = [];
    for (dynamic v in valuesData) {
      switch (v.runtimeType) {
        case double:
          out.add(v as double);
          break;
        case int:
          out.add((v as int).toDouble());
          break;
        default:
          throw Exception(
              "Unknown data type in values: ${v.toString()} ('${v.runtimeType}')");
      }
    }
    return out;
  }
}
