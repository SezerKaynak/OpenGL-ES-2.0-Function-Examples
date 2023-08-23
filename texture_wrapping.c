// Sezer KAYNAK

#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Shader kaynak kodları
const char* vertexShaderSource =
    "attribute vec3 vertexPosition;\n"
    "attribute vec2 vertexTexCoord;\n"
    "varying vec2 texCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(vertexPosition, 1.0);\n"
    "    texCoord = vertexTexCoord;\n"
    "}\n";

// Fragment Shader kaynak kodları
const char* fragmentShaderSource =
    "precision mediump float;\n"
    "varying vec2 texCoord;\n"
    "uniform sampler2D textureSampler;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(textureSampler, texCoord);\n"
    "}\n";

// Shader oluşturma fonksiyonu
GLuint createShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Shader derleme hatalarını kontrol etme
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader derleme hatası:\n%s\n", infoLog);
        return 0;
    }

    return shader;
}
// Texture ve vertex buffer nesnelerini tanımlama
GLuint vertexBuffer, texture;

int main() {

    const char* textureFile = "assets/wall2.png";

    // GLFW'yi başlatma
    if (!glfwInit()) {
        printf("GLFW başlatılamadı.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // GLFW penceresini oluşturma
    GLFWwindow* window = glfwCreateWindow(1400, 500, "Texture Wrapping", NULL, NULL);
    if (!window) {
        printf("GLFW penceresi oluşturulamadı.\n");
        glfwTerminate();
        return -1;
    }

    // OpenGL bağlantısını yapma
    glfwMakeContextCurrent(window);

    // Vertex Shader ve Fragment Shader oluşturma
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Shader programını oluşturma ve bağlama
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    //glGetShaderPrecisionFormat fonksiyonu ile hassasiyet formatını alma
    GLint range[2];
    GLint precision;

    glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, range, &precision);
    printf("GL_HIGH_FLOAT:\n");
    printf("Minimum Değer: %d\n", range[0]);
    printf("Maksimum Değer: %d\n", range[1]);
    printf("Kesirli Hane Hassasiyeti: %d\n\n", precision);


    // Üçgenin köşe koordinatları ve texture koordinatları
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 2.0f, 0.0f, // Doku koordinatları düzenlendi
         0.0f,  1.0f, 0.0f, 1.0f, 1.0f // Doku koordinatları düzenlendi
    };
    
    // Vertex Buffer oluşturma ve vertex verilerini yükleme
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex Attribute Pointerları ayarlama
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture yükleme
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, channels;
    stbi_uc* image = stbi_load(textureFile, &width, &height, &channels, STBI_rgb_alpha);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    // Ana döngü
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Texture binding
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

        // İlk üçgeni çizme (GL_REPEAT ile)
        glViewport(50, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // İkinci üçgeni çizme (GL_CLAMP_TO_EDGE ile)
        glViewport(500, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Üçüncü üçgeni çizme (GL_MIRRORED_REPEAT ile)
        glViewport(950, 50, 400, 400);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Çift tamponlama ve olayları işleme
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Temizleme
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    // Shader'ları silme (artık bağlı değiller)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // GLFW'yi temizleme ve çıkış
    glfwTerminate();
    return 0;
}
