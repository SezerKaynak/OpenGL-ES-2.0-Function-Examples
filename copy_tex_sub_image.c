// Sezer KAYNAK

#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Vertex Shader kaynak kodları
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
    "uniform sampler2D textureSampler1;\n"
    "void main() {\n"
    "   gl_FragColor = texture2D(textureSampler1, texCoord);\n"
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

// Vertex ve Texture buffer nesnelerini tanımlama
GLuint vertexBuffer, texture1, texture2;

// Texture yükleme fonksiyonu
void loadTexture(const char* filename, GLuint* texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, channels;
    stbi_uc* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (image == NULL) {
        printf("Resim yüklenemedi!!\n");
        return;
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(image);
}

int main() {

    const char* textureFile1 = "assets/brick.jpg";
    const char* textureFile2 = "assets/container.jpg";

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
    GLFWwindow* window = glfwCreateWindow(950, 500, "Copy Tex Sub Image", NULL, NULL);
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


    // Üçgenin köşe koordinatları ve texture koordinatları
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.5f, 1.0f
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

    // Texture'ları yükleme
    loadTexture(textureFile1, &texture1);
    loadTexture(textureFile2, &texture2);

    GLfloat minFilterValue;
    glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilterValue);
    printf("%s%f\n", "Min Filter Value: ", minFilterValue);
    
    // Ana döngü
     while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        // İlk üçgeni ilk texture ile çizme
        
        glActiveTexture(GL_TEXTURE0); // İlk texture aktif ediliyor
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler1"), 0);
        glViewport(50, 50, 400, 400);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // İkinci üçgeni brick texture ile çizme
        glActiveTexture(GL_TEXTURE1); // İkinci texture aktif ediliyor
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler1"), 1);
        glViewport(500, 50, 400, 400);
        
        // Eski üçgenin Texture'ını kopyalama
        glBindTexture(GL_TEXTURE_2D, texture2); // Kopyalanacak Texture
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 50, 50, 200, 200);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindTexture(GL_TEXTURE_2D, 0);
        // Çift tamponlama ve olayları işleme
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Temizleme
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteProgram(shaderProgram);
    // Shader'ları silme (artık bağlı değiller)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // GLFW'yi temizleme ve çıkış
    glfwTerminate();
    return 0;
}
