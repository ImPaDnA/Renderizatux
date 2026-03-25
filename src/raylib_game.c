#include "raylib.h"
#include <stdbool.h> // Necesario en C puro para usar true/false

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void)
{
    // Configuración inicial de la ventana
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    // Habilitar redimensionamiento y anti-aliasing
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Renderizatux - Visor 3D");

    // Configuración de la cámara 3D
    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Variables del modelo y textura
    Model model = { 0 };
    Texture2D texture = { 0 };
    bool modelLoaded = false;
    
    // Datos del archivo
    const char* fileName = "Ningún modelo cargado";
    const char* fileType = "N/A";

    SetTargetFPS(60);               

    // Bucle principal
    while (!WindowShouldClose())    
    {
        // 1. Lógica de Importación (Drag & Drop para PC)
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count > 0)
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") || 
                    IsFileExtension(droppedFiles.paths[0], ".gltf") || 
                    IsFileExtension(droppedFiles.paths[0], ".glb"))
                {
                    if (modelLoaded) UnloadModel(model);
                    model = LoadModel(droppedFiles.paths[0]);
                    fileName = GetFileName(droppedFiles.paths[0]);
                    fileType = GetFileExtension(droppedFiles.paths[0]);
                    modelLoaded = true;
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png") || 
                         IsFileExtension(droppedFiles.paths[0], ".jpg"))
                {
                    if (modelLoaded)
                    {
                        if (texture.id != 0) UnloadTexture(texture);
                        texture = LoadTexture(droppedFiles.paths[0]);
                        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                    }
                }
            }
            UnloadDroppedFiles(droppedFiles);
        }

        // Lógica de cámara (Permite rotar alrededor del modelo con el dedo/mouse)
        UpdateCamera(&camera, CAMERA_ORBITAL);

        // 2. Dibujado de la interfaz y el 3D
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (modelLoaded)
            {
                BeginMode3D(camera);
                    DrawModel(model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
                    DrawGrid(10, 1.0f); // Cuadrícula de referencia
                EndMode3D();
            }
            else
            {
                DrawText("Esperando modelo 3D (.obj, .gltf)", 150, screenHeight/2, 20, DARKGRAY);
            }

            // --- INTERFAZ GRÁFICA (GUI) ---
            GuiPanel((Rectangle){ 10, 10, 250, 200 }, "Panel de Control");
            
            // Botones
            if (GuiButton((Rectangle){ 20, 40, 230, 30 }, "Importar Modelo")) {
                // Nota: Abrir el gestor de archivos de Android nativo requiere JNI extra.
                // Por ahora la base visual de la interfaz está lista.
            }
            if (GuiButton((Rectangle){ 20, 80, 230, 30 }, "Exportar (WIP)")) {
            }
            if (GuiButton((Rectangle){ 20, 120, 230, 30 }, "Vista Previa Libre")) {
                camera.position = (Vector3){ 5.0f, 5.0f, 5.0f }; // Reiniciar cámara
            }

            // Datos del modelo
            DrawText(TextFormat("Nombre: %s", fileName), 20, 160, 10, DARKGRAY);
            DrawText(TextFormat("Tipo: %s", fileType), 20, 180, 10, DARKGRAY);

        EndDrawing();
    }

    // Limpieza de memoria 
    if (modelLoaded) UnloadModel(model);
    if (texture.id != 0) UnloadTexture(texture);
    CloseWindow();        

    return 0;
}
