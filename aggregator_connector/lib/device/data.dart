import 'dart:async';
import 'dart:developer' as dev;

import 'package:aggregator_connector/device/data_field.dart';
import 'package:aggregator_connector/translate.dart';
import 'package:flutter/material.dart';
import 'package:input_slider/input_slider.dart';
import 'package:provider/provider.dart';

import '../http_path.dart';
import 'data_chart.dart';

class DeviceDataContext extends ChangeNotifier {
  final JsonApi api;
  List<Map<String, dynamic>> data = [
    {"type": "single", "name": "Loading...", "value": "", "unit": ""}
  ];
  int _updating = 0;
  Timer? timer;
  Duration _refreshInterval = const Duration(milliseconds: 1000);
  bool _disposed = false;

  DeviceDataContext({
    required this.api,
    Duration refreshInterval = const Duration(milliseconds: 1000),
  }) : _refreshInterval = refreshInterval {
    setPeriodicUpdate(_refreshInterval);
  }

  void update() async {
    if (_updating > 0) {
      dev.log("WARN: DeviceDataContext already updating.");
      return;
    }
    ++_updating;
    dev.log("INFO: DeviceDataContext updating.");

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
      if (!_disposed) {
        notifyListeners();
      }
    } finally {
      --_updating;
    }
  }

  void setPeriodicUpdate(Duration duration) {
    timer?.cancel();
    _refreshInterval = duration;
    timer = Timer.periodic(duration, (timer) {
      update();
    });
  }

  @override
  void dispose() {
    _disposed = true;
    timer?.cancel();
    super.dispose();
  }
}

class DeviceData extends StatelessWidget {
  final DeviceDataContext ctx;
  // final Map<String, DeviceChart> chartCache = {};

  const DeviceData({required this.ctx, super.key});

  @override
  Widget build(BuildContext context) {
    final ret = Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            _buildRefreshDelaySelector(),
            Row(
              children: [
                ConnectionTypeSelector(
                  ctx: ctx,
                ),
              ],
            ),
            ChangeNotifierProvider<DeviceDataContext>(
              create: (_) => ctx,
              child: Consumer<DeviceDataContext>(builder: tableBuilder),
            ),
          ],
        ));
    return ret;
  }

  Row _buildRefreshDelaySelector() {
    return Row(
      children: [
        const Expanded(
          flex: 1,
          child: Text("Refresh delay [s]"),
        ),
        Expanded(
          flex: 3,
          child: InputSlider(
            defaultValue: ctx._refreshInterval.inMilliseconds.toDouble() / 1000,
            onChange: (e) => null,
            onChangeEnd: (v) {
              ctx.setPeriodicUpdate(Duration(milliseconds: (v * 1000).round()));
            },
            min: 0.1,
            max: 5,
            division: 98,
          ),
        ),
      ],
    );
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
    final String xLabel;

    if (timestamps != null) {
      final v = _parseValues(timestamps); // in [us]
      final offset = v.first;
      final vRelative = v.map((e) => e - offset);
      final lastVal = vRelative.last;
      final int divider;

      if (lastVal < 2000) {
        xLabel = "Time [us]";
        divider = 1;
      } else if (lastVal < 2000000) {
        xLabel = "Time [ms]";
        divider = 1000;
      } else {
        xLabel = "Time [s]";
        divider = 1000000;
      }
      x = List<double>.from(vRelative.map((e) => e / divider));
    } else {
      x = List<double>.generate(y.length, (i) => i + 1);
      xLabel = "Indexes";
    }

    String name = data["name"];
    DeviceChart chart = DeviceChart(
      x: x,
      y: y,
      yMin: data["min"]?.toDouble(),
      yMax: data["max"]?.toDouble(),
      title: tr(name),
      xLabel: xLabel,
      yLabel: "Values [${data["unit"]}]",
    );

    return chart;
  }

  List<double> _parseValues(List<dynamic> valuesData) {
    List<double> out = [];
    for (dynamic v in valuesData) {
      switch (v.runtimeType) {
        case const (double):
          out.add(v as double);
          break;
        case const (int):
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

class ConnectionTypeSelector extends StatefulWidget {
  const ConnectionTypeSelector({
    super.key,
    required this.ctx,
  });

  final DeviceDataContext ctx;

  @override
  State<StatefulWidget> createState() {
    return _ConnectionTypeSelector();
  }
}

class _ConnectionTypeSelector extends State<ConnectionTypeSelector> {
  String selectedValue = 'http';

  @override
  Widget build(BuildContext context) {
    return DropdownButton<String>(
      style: const TextStyle(color: Colors.black),
      value: selectedValue,
      items: const [
        DropdownMenuItem<String>(
          value: 'http',
          child: Text("Use HTTP"),
        ),
        DropdownMenuItem<String>(
          value: 'wss',
          child: Text("Use WebSocket"),
        ),
      ],
      onChanged: (value) {
        switch (value) {
          case 'http':
            widget.ctx.api.handlers.useHttp();
            break;
          case 'wss':
            widget.ctx.api.handlers.useWebSocket();
            break;
          default:
            break;
        }
        setState(() {
          selectedValue = value!;
        });
      },
    );
  }
}
