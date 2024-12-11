#version 330

uniform sampler2D texture0;
uniform vec4 colDiffuse;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord) * fragColor * colDiffuse;
    float r = color.r;
    float g = color.g;
    float b = color.b;

    float newR = (0.95 * r) + (0.05 * g) + (0.0 * b);
    float newG = (0.0 * r) + (0.43333 * g) + (0.56667 * b);
    float newB = (0.0 * r) + (0.475 * g) + (0.525 * b);

    finalColor = vec4(newR, newG, newB, color.a);
}
