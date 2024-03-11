#pragma once
#define STORMWATCH_DEPRECATED_MESSAGE "This feature is deprecated and will be removed in future updates."
#define STORMWATCH_CONCAT(A, B) STORMWATCH_CONCAT_INNER(A, B)
#define STORMWATCH_CONCAT_INNER(A, B) A ## B
#define STORMWATCH_SELECT(_1, _2, NAME, ...) NAME
#if _MSC_VER
	#define STORMWATCH_DEPRECATED_1() __declspec(deprecated(STORMWATCH_DEPRECATED_MESSAGE))
	#define STORMWATCH_DEPRECATED_2(MSG) __declspec(deprecated(MSG))
#elif defined(__GNUC__) || defined(__clang__)
	#define STORMWATCH_DEPRECATED_1() __attribute__((deprecated(STORMWATCH_DEPRECATED_MESSAGE)))
	#define STORMWATCH_DEPRECATED_2(MSG) __attribute__((deprecated(MSG)))
#else
	#define STORMWATCH_DEPRECATED_1()
	#define STORMWATCH_DEPRECATED_2(MSG)
#endif
#define STORMWATCH_DEPRECATED(...) STORMWATCH_CONCAT(STORMWATCH_DEPRECATED_, STORMWATCH_SELECT(__VA_ARGS__, 2, 1, ))(__VA_ARGS__)