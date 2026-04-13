{
	"targets": [
		{
			"target_name": "addon",
			"sources": [
				"src/wrapper.cpp"
			],
			"include_dirs": [
				"<!(node -p \"require('node-addon-api').include\")",
				"src"
			],
			"dependencies": [
				"<!(node -p \"require('node-addon-api').gyp\")"
			],
			"cflags_cc!": [ "-fno-exceptions" ],
			"defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
		}
	]
}