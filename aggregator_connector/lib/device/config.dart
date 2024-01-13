import 'package:aggregator_connector/device/config_field.dart';
import 'package:flutter/widgets.dart';
import 'package:provider/provider.dart';

import '../http_path.dart';

class DeviceConfigContext extends ChangeNotifier {
  final JsonApi api;
  int _posting = 0;
  int _reloading = 0;
  List<Widget> widgets = [const Text("Initializing...")];
  List<Widget> postErrors = [];

  DeviceConfigContext({required this.api});

  void post(
      {required String name, String? unit, required dynamic value}) async {

    if (_posting > 0) {

      return;
    }
    ++_posting;
    postErrors.clear();
    widgets.clear();
    widgets.add(const Text("Loading..."));
    notifyListeners();

    var data = {"name": name, "value": value};
    if (unit != null) {
      data["unit"] = unit;
    }
    try {
      await api.post([data]);
    } catch (e) {
      widgets.clear();
      postErrors.add(DeviceConfigErrorRow(
        description: "Error: $e",
      ));
      notifyListeners();
    }
    await reload();
    --_posting;
  }

  Future<void> reload() async {

    if (_reloading > 0) {

      return;
    }

    ++_reloading;

    HttpJsonResponse resp;
    try {
      resp = await api.get();
    } catch (e) {
      widgets.clear();
      widgets.add(DeviceConfigErrorRow(description: "Error: $e"));
      notifyListeners();
      --_reloading;
      return reload();
    }
    widgets.clear();
    final respData = resp.asList();
    for (var el in respData) {
      switch (el["type"]) {
        case "enum":
          widgets.add(DeviceConfigEnumRow(data: el, ctx: this));
          break;
        case "integer":
        case "float":
          widgets.add(DeviceConfigNumberRow(
            data: el,
            ctx: this,
          ));
          break;
        default:
      }
    }
    notifyListeners();
    --_reloading;
  }
}

class DeviceConfig extends StatelessWidget {
  final DeviceConfigContext ctx;

  const DeviceConfig({required this.ctx, super.key});

  @override
  Widget build(BuildContext context) {
    final ret = Padding(
        padding: const EdgeInsets.all(16.0),
        child: ChangeNotifierProvider<DeviceConfigContext>(
          create: (_) => ctx,
          child: Consumer<DeviceConfigContext>(builder: tableBuilder),
        ));
    return ret;
  }

  Widget tableBuilder(
      BuildContext context, DeviceConfigContext ctx, Widget? child) {
    return Column(
      children: ctx.postErrors + ctx.widgets,
    );
  }
}
