import 'package:aggregator_connector/device/config.dart';
import 'package:flutter/material.dart';
import 'package:input_slider/input_slider.dart';
import '../translate.dart';

double mod(double value, double divider) {
  // '%' does not work with double values, but this works!
  final double divResult = value / divider;
  final double rest = divResult - divResult.floorToDouble();
  if (rest > 0.999999) {
    return 0;
  }
  return rest * divider;
}

class DeviceConfigErrorRow extends StatelessWidget {
  final String description;
  const DeviceConfigErrorRow({LocalKey? key, required this.description})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: Text(
            "Error: $description",
            style: const TextStyle(
              color: Color(0xFFCC0000),
            ),
          ),
        ),
      ],
    );
  }
}

class DeviceConfigNumberRow extends StatelessWidget {
  final DeviceConfigContext ctx;
  final Map<String, dynamic> data;
  const DeviceConfigNumberRow(
      {LocalKey? key, required this.data, required this.ctx})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    final double value = data["value"].toDouble();
    final constrains = data["constrains"] as Map<String, dynamic>;
    final double vMin = constrains["min"].toDouble();
    final double vStep = constrains["step"].toDouble();
    final double vMax = constrains["max"].toDouble() -
        mod(constrains["max"].toDouble() - vMin, vStep);
    final int vDivisions = ((vMax - vMin) / vStep).floor();

    if (value < vMin || value > vMax || mod(value - vMin, vStep) != 0) {
      return Row(
        children: [
          Expanded(
            flex: 1,
            child: Text(tr(data["name"])),
          ),
          Expanded(
            flex: 3,
            child: Text(
              "Error: value $value does not satisfy constrains: min $vMin, max $vMax, step $vStep.",
              style: const TextStyle(
                color: Color(0xFFCC0000),
              ),
            ),
          )
        ],
      );
    }
    return Row(
      children: [
        Expanded(
          flex: 1,
          child: Text("${tr(data["name"])} [${data["unit"]}]"),
        ),
        Expanded(
          flex: 3,
          child: InputSlider(
            defaultValue: value,
            onChange: (e) => null,
            onChangeEnd: (v) {

              ctx.post(name: data["name"], unit: data["unit"], value: v);
            },
            min: vMin,
            max: vMax,
            division: vDivisions,
          ),
        ),
      ],
    );
  }
}

class DeviceConfigEnumRow extends StatelessWidget {
  final DeviceConfigContext ctx;
  final Map<String, dynamic> data;
  const DeviceConfigEnumRow(
      {LocalKey? key, required this.data, required this.ctx})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    final List<DropdownMenuItem<String>> enumStrings =
        (data["constrains"] as List<dynamic>)
            .map((e) => DropdownMenuItem<String>(
                  value: e.toString(),
                  child: Text(e.toString()),
                ))
            .toList();
    return Row(
      children: [
        Expanded(
          flex: 1,
          child: Text(tr(data["name"])),
        ),
        // Expanded(
        //   flex: 3,
        //   child: Text("${data["value"]}"),
        // ),
        Expanded(
          flex: 3,
          child: Row(
            children: [
              DropdownButton<String>(
                style: const TextStyle(color: Colors.black),
                value: data["value"],
                items: enumStrings,
                onChanged: (v) {
                  ctx.post(name: data["name"], value: v);
                },
              ),
            ],
          ),
        ),
      ],
    );
  }
}
