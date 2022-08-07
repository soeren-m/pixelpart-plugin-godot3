enum InterpolationType {
	NONE = 0,
	LINEAR = 1,
	SPLINE = 2
}

enum BlendModeType {
	NORMAL = 0,
	ADDITIVE = 1,
	SUBTRACTIVE = 2
}

enum EmitterShapeType {
	POINT = 0,
	LINE = 1,
	ELLIPSE = 2,
	RECTANGLE = 3,
	PATH = 4
}

enum EmitterDistributionType {
	UNIFORM = 0,
	CENTER = 1,
	CENTER_REVERSE = 2,
	BOUNDARY = 3
}

enum EmitterSpawnModeType {
	FIXED = 0,
	OUT = 1,
	IN = 2
}

enum EmitterInstantiationModeType {
	CONTINUOUS = 0,
	BURST = 1,
	BURST_DEATH = 2
}

enum RotationModeType {
	ANGLE = 0,
	VELOCITY = 1,
	ALIGN = 2,
	ALIGN_PATH = 3
}

enum ForceType {
	POINT = 0,
	AREA = 1
}
