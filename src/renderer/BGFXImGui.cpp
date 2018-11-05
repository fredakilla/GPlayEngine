#if 0
#include "BGFXImGui.h"

#include "../core/Base.h"
#include <dear-imgui/imgui.h>
#include <brtshaderc/brtshaderc.h>
#include <bx/math.h>

namespace gplay {

static const bgfx::ViewId GP_IMGUI_VIEW_ID = 255;

void BGFXImGui::imguiInit()
{
    unsigned char* data;
    int width, height;

    ImGuiIO& io = ImGui::GetIO();

    // Setup vertex declaration
    imguiVertexDecl
        .begin()
        .add( bgfx::Attrib::Position, 2, bgfx::AttribType::Float )
        .add( bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float )
        .add( bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true )
        .end();

    // Create font
    io.Fonts->AddFontDefault();
    io.Fonts->GetTexDataAsRGBA32( &data, &width, &height );
    imguiFontTexture = bgfx::createTexture2D( ( uint16_t )width, ( uint16_t )height, false, 1, bgfx::TextureFormat::BGRA8, 0, bgfx::copy( data, width*height * 4 ) );
    imguiFontUniform = bgfx::createUniform( "s_tex", bgfx::UniformType::Int1 );

    // Create shader program
    const bgfx::Memory* memVsh = shaderc::compileShader(shaderc::ST_VERTEX, "res/core/shaders/imgui.vert", 0);
    const bgfx::Memory* memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, "res/core/shaders/imgui.frag", 0);
    bgfx::ShaderHandle vs = bgfx::createShader(memVsh);
    bgfx::ShaderHandle fs = bgfx::createShader(memFsh);
    imguiProgram = bgfx::createProgram( vs, fs, true );
}

void BGFXImGui::imguiReset( uint16_t width, uint16_t height )
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
    io.DeltaTime   = 1.0f / 60.0f;
    io.IniFilename = NULL;
}

void BGFXImGui::imguiRender( ImDrawData* drawData )
{
    const ImGuiIO& io = ImGui::GetIO();
    const float width  = io.DisplaySize.x;
    const float height = io.DisplaySize.y;

    bgfx::setViewName(GP_IMGUI_VIEW_ID, "ImGui");
    bgfx::setViewMode(GP_IMGUI_VIEW_ID, bgfx::ViewMode::Sequential);

    // set view
    const bgfx::Caps* caps = bgfx::getCaps();
    {
        float ortho[16];
        bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(GP_IMGUI_VIEW_ID, NULL, ortho);
        bgfx::setViewRect(GP_IMGUI_VIEW_ID, 0, 0, uint16_t(width), uint16_t(height) );
    }


    // draw
    for ( int ii = 0, num = drawData->CmdListsCount; ii < num; ++ii )
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        const ImDrawList* drawList = drawData->CmdLists[ii];
        uint32_t numVertices = ( uint32_t )drawList->VtxBuffer.size();
        uint32_t numIndices  = ( uint32_t )drawList->IdxBuffer.size();

        if ( !bgfx::getAvailTransientVertexBuffer( numVertices, imguiVertexDecl ) || !bgfx::getAvailTransientIndexBuffer( numIndices ) )
        {
            break;
        }

        bgfx::allocTransientVertexBuffer( &tvb, numVertices, imguiVertexDecl );
        bgfx::allocTransientIndexBuffer( &tib, numIndices );

        ImDrawVert* verts = ( ImDrawVert* )tvb.data;
        memcpy( verts, drawList->VtxBuffer.begin(), numVertices * sizeof( ImDrawVert ) );

        ImDrawIdx* indices = ( ImDrawIdx* )tib.data;
        memcpy( indices, drawList->IdxBuffer.begin(), numIndices * sizeof( ImDrawIdx ) );

        uint32_t offset = 0;
        for ( const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd )
        {
            if ( cmd->UserCallback )
            {
                cmd->UserCallback( drawList, cmd );
            }
            else if ( 0 != cmd->ElemCount )
            {
                uint64_t state = 0
                        | BGFX_STATE_WRITE_RGB
                        | BGFX_STATE_WRITE_A
                        | BGFX_STATE_MSAA
                        ;

                bgfx::TextureHandle th = imguiFontTexture;
                if ( cmd->TextureId != NULL )
                {
                    union { ImTextureID ptr; struct { uint16_t flags; bgfx::TextureHandle handle; } s; } texture = { cmd->TextureId };
                    state |= BGFX_STATE_BLEND_FUNC( BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA );
                    th = texture.s.handle;
                }
                else
                {
                    state |= BGFX_STATE_BLEND_FUNC( BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA );
                }
                const uint16_t xx = uint16_t( fmax( cmd->ClipRect.x, 0.0f ) );
                const uint16_t yy = uint16_t( fmax( cmd->ClipRect.y, 0.0f ) );
                bgfx::setScissor( xx, yy, uint16_t( fmin( cmd->ClipRect.z, 65535.0f ) - xx ), uint16_t( fmin( cmd->ClipRect.w, 65535.0f ) - yy ) );
                bgfx::setState( state );
                bgfx::setTexture( 0, imguiFontUniform, th );
                bgfx::setVertexBuffer( 0, &tvb, 0, numVertices );
                bgfx::setIndexBuffer( &tib, offset, cmd->ElemCount );
                bgfx::submit(GP_IMGUI_VIEW_ID, imguiProgram );
            }

            offset += cmd->ElemCount;
        }
    }
}

void BGFXImGui::imguiShutdown()
{
    bgfx::destroy( imguiFontUniform );
    bgfx::destroy( imguiFontTexture );
    bgfx::destroy( imguiProgram );
    ImGui::DestroyContext();
}


}
#endif
