
uniform sampler2D diffuse;
uniform vec3 tint;

varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(color.r * tint.r/0.65, color.g * tint.g/0.65, color.b * tint.b/0.65, color.a);
}
