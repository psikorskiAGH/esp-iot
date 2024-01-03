import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';
import 'package:flutter/material.dart';

import '../http_path.dart';
import '../translate.dart';

class DeviceInfoContext extends ChangeNotifier {
  final JsonApi api;
  Map<String, String> data = {"Loading...": ""};

  DeviceInfoContext({required this.api});

  void update() async {
    print("Update");
    HttpJsonResponse resp;
    try {
      resp = await api.get();
    } catch (e) {
      data.clear();
      data["Error"] = e.toString();
      notifyListeners();
      return;
    }
    final respData = resp.asMap();
    data.clear();
    for (var el in respData.entries) {
      data[el.key.toString()] = el.value.toString();
    }
    notifyListeners();
  }
}

class DeviceInfo extends StatelessWidget {
  final DeviceInfoContext ctx;

  const DeviceInfo({required this.ctx, super.key});

  @override
  Widget build(BuildContext context) {
    final ret = Padding(
        padding: const EdgeInsets.all(16.0),
        child: ChangeNotifierProvider<DeviceInfoContext>(
          create: (_) => ctx,
          child: Consumer<DeviceInfoContext>(builder: tableBuilder),
        ));
    ctx.update();
    return ret;
  }

  Widget tableBuilder(
      BuildContext context, DeviceInfoContext ctx, Widget? child) {
    var ch = <TableRow>[];
    for (var el in ctx.data.entries) {
      ch.add(
        TableRow(
          children: <Widget>[
            TableCell(
              child: Padding(
                padding: const EdgeInsets.only(right: 8.0),
                child: Align(
                  alignment: Alignment.centerLeft,
                  child: Text(
                    tr(el.key.toString()),
                  ),
                ),
              ),
            ),
            TableCell(
              child: Text(
                el.value.toString(),
              ),
            ),
          ],
        ),
      );
    }
    return Table(
      children: ch,
      columnWidths: const <int, TableColumnWidth>{
        0: FixedColumnWidth(100),
      },
    );
  }
}

// class DeviceInfo extends StatefulWidget {
//   final HttpJsonApi api;

//   DeviceInfo(this.api, {super.key});
//   @override
//   State<DeviceInfo> createState() => _DeviceInfoState();
// }

// class _DeviceInfoState extends State<DeviceInfo> {
//   var children = <TableRow>[];

//   void _load() async {
//     var resp;
//     try {
//       resp = await widget.api.get();
//     } catch (e) {
//       setState(() {
//         children.add(TableRow(children: <Widget>[Text("Error: $e")]));
//       });
//       return;
//     }
//     final data = await resp.asMap();
//     setState(() {
//       for (var el in data.entries) {
//         children.add(TableRow(children: <Widget>[
//           TableCell(
//               child: Padding(
//             padding: const EdgeInsets.only(right: 8.0),
//             child: Align(
//                 alignment: Alignment.centerLeft,
//                 child: Text(
//                     keyTranslation[el.key.toString()] ?? el.key.toString())),
//           )),
//           TableCell(child: Text(el.value.toString())),
//         ]));
//       }
//     });
//   }

//   @override
//   void initState() {
//     super.initState();
//     _load();
//   }

//   @override
//   Widget build(BuildContext context) {
//     return Padding(
//       padding: const EdgeInsets.all(16.0),
//       child: Table(
//         children: children,
//         columnWidths: const <int, TableColumnWidth>{
//           0: FixedColumnWidth(100),
//         },
//       ),
//     );
//   }
// }
