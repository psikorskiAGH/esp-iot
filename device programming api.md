
## ApiError

- `ApiError(code: int, description: str)`
- `code: int` - HTTP code
- `description: str`
- `getResponseData() -> JSON-like obj`

### MethodNotAllowedApiError(ApiError)
- `MethodNotAllowedApiError(description: str)`
- `code = 405`

### etc.

## ApiElement

*Abstract class*

private:
- `pathName: str`

public:
- `ApiElement(pathName: str)`
- `registerChild(child: ApiElement) -> None`
- `getElementByPath(path: Path) -> ApiElement`
- {abstract} `GET() -> JSON-like obj`
- {abstract} `POST(data: JSON-like obj) -> JSON-like obj`


## Device(ApiElement)

Must invoke `self.registerChild()` for each element
- `Device(pathName: str)`

private:
- `id: str`
- `name: str`
- `model: str`

public:
- `DeviceInfo(pathName: str)`
<!-- - `GET() -> JSON-like obj` -->
- `POST(data: JSON-like obj) -> ERROR MethodNotAllowedApiError`


### DeviceData(ApiElement)

