import 'package:aggregator_connector/device/config.dart';
import 'package:flutter/material.dart';
import 'package:input_slider/input_slider.dart';
import '../translate.dart';


class ConfigDataErrorRow extends StatelessWidget {
  final String description;
  const ConfigDataErrorRow({LocalKey? key, required this.description})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: Text("Error: $description"),
        ),
      ],
    );
  }
}

class ConfigDataNumberRow extends StatelessWidget {
  final DeviceConfigContext ctx;
  final Map<String, dynamic> data;
  const ConfigDataNumberRow({LocalKey? key, required this.data, required this.ctx})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    final double value = data["value"].toDouble();
    final constrains = data["constrains"] as Map<String, dynamic>;
    final double vMin = constrains["min"].toDouble();
    final double vStep = constrains["step"].toDouble();
    final double vMax = constrains["max"].toDouble() -
        (constrains["max"].toDouble() - vMin) % vStep as double;
    final int vDivisions = ((vMax - vMin) / vStep).floor();
    print("Divs: $vDivisions");
    if (value < vMin || value > vMax || (value - vMin) % vStep != 0) {
      return Row(
        children: [
          Expanded(
            flex: 1,
            child: Text(tr(data["name"])),
          ),
          Expanded(
            flex: 3,
            child: Text(
                "Error: value $value does not satisfy constrains: min $vMin, max $vMax, step $vStep."),
          )
        ],
      );
    }
    return Row(
      children: [
        Expanded(
          flex: 1,
          child: Text(tr(data["name"])),
        ),
        // Expanded(
        //   flex: 3,
        //   child: Text("${data["value"]} [${data["unit"]}]"),
        // ),
        // Expanded(
        //   flex: 3,
        //   child: Slider(
        //     value: value,
        //     onChanged: (e) => print(e),
        //     onChangeEnd: (value) => print("End: $value"),
        //     min: vMin,
        //     max: vMax,
        //     divisions: vDivisions,
        //   ),
        // ),
        Expanded(
          flex: 3,
          child: InputSlider(
            defaultValue: value,
            onChange: (e) => null,
            onChangeEnd: (v) {
              print("End: $v");
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

class ConfigDataEnumRow extends StatelessWidget {
  final DeviceConfigContext ctx;
  final Map<String, dynamic> data;
  const ConfigDataEnumRow({LocalKey? key, required this.data, required this.ctx})
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
                  print(v);
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
