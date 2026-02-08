import { useState } from "react";
import { TEXTURES, type TextureType, type ShapeType, type InputMode } from "../types";

interface SidebarProps {
    gravity: number;
    setGravity: (v: number) => void;
    restitution: number;
    setRestitution: (v: number) => void;
    friction: number;
    setFriction: (v: number) => void;
    debugMode: boolean;
    setDebugMode: (v: boolean) => void;
    selectedTexture: TextureType;
    setSelectedTexture: (v: TextureType) => void;
    selectedShape: ShapeType;
    setSelectedShape: (v: ShapeType) => void;
    inputMode: InputMode;
    setInputMode: (v: InputMode) => void;
    skyColor: string;
    setSkyColor: (v: string) => void;
    planeColor: string;
    setPlaneColor: (v: string) => void;
    gridColor: string;
    setGridColor: (v: string) => void;
    onSpawn: (type: ShapeType) => void;
    onClear: () => void;
}

export const Sidebar = ({
    gravity, setGravity,
    restitution, setRestitution,
    friction, setFriction,
    debugMode, setDebugMode,
    selectedTexture, setSelectedTexture,
    selectedShape, setSelectedShape,
    inputMode, setInputMode,
    skyColor, setSkyColor,
    planeColor, setPlaneColor,
    gridColor, setGridColor,
    onSpawn, onClear
}: SidebarProps) => {
    const [isMinimized, setIsMinimized] = useState(false);

    return (
      <div className={`sidebar ${isMinimized ? 'minimized' : ''}`}>
        <div className="sidebar-header">
            {!isMinimized && <h2>Physics Workbench</h2>}
            <button 
                className="minimize-btn" 
                onClick={() => setIsMinimized(!isMinimized)}
                aria-label={isMinimized ? "Expand sidebar" : "Collapse sidebar"}
            >
                {isMinimized ? '☰' : '✕'}
            </button>
        </div>

        {!isMinimized && (
        <div className="sidebar-content">
                <div className="section-title">Controls</div>
                <div className="control-group">
                    <label>Input Mode</label>
                    <div className="row-btns">
                        <button className={`toggle-btn ${inputMode === 'keyboard' ? 'active' : ''}`} onClick={() => setInputMode('keyboard')}>⌨️ Keyboard</button>
                        <button className={`toggle-btn ${inputMode === 'controller' ? 'active' : ''}`} onClick={() => setInputMode('controller')}>🎮 Gamepad</button>
                    </div>
                </div>
                
                <hr />

                <div className="section-title">Physics</div>
                <div className="control-group">
                    <label>Gravity: {gravity.toFixed(1)}</label>
                    <input type="range" min="-20" max="0" step="0.1" value={gravity} onChange={e => setGravity(Number(e.target.value))} />
                </div>
                <div className="control-group">
                    <label>Bounciness: {restitution.toFixed(1)}</label>
                    <input type="range" min="0" max="1.5" step="0.1" value={restitution} onChange={e => setRestitution(Number(e.target.value))} />
                </div>
                <div className="control-group">
                    <label>Friction: {friction.toFixed(1)}</label>
                    <input type="range" min="0" max="1" step="0.1" value={friction} onChange={e => setFriction(Number(e.target.value))} />
                </div>

                <hr />

                <div className="section-title">Scene Colors</div>
                <div className="color-row">
                    <label>Sky</label>
                    <input type="color" value={skyColor} onChange={e => setSkyColor(e.target.value)} />
                </div>
                <div className="color-row">
                    <label>Ground</label>
                    <input type="color" value={planeColor} onChange={e => setPlaneColor(e.target.value)} />
                </div>
                <div className="color-row">
                    <label>Grid</label>
                    <input type="color" value={gridColor} onChange={e => setGridColor(e.target.value)} />
                </div>

                <hr />
                
                <div className="section-title">Display</div>
                <div className="control-group">
                    <button className={`toggle-btn ${debugMode ? 'active' : ''}`} onClick={() => setDebugMode(!debugMode)}>
                        {debugMode ? '🔲 Wireframe ON' : '🔲 Wireframe OFF'}
                    </button>
                </div>

                <hr />

                <div className="section-title">Texture</div>
                <div className="texture-grid">
                    {(Object.keys(TEXTURES) as TextureType[]).map(tex => (
                        <button key={tex} className={`texture-btn ${selectedTexture === tex ? 'active' : ''}`} onClick={() => setSelectedTexture(tex)} style={{ backgroundImage: `url(${TEXTURES[tex]})` }} />
                    ))}
                </div>

                <hr />

                <div className="section-title">Spawn Objects</div>
                <div className="action-buttons">
                    <div className="spawn-row">
                        <button className={`spawn-btn ${selectedShape === 'box' ? 'active' : ''}`} onClick={() => { setSelectedShape('box'); onSpawn('box'); }}>📦 Box</button>
                        <button className={`spawn-btn ${selectedShape === 'sphere' ? 'active' : ''}`} onClick={() => { setSelectedShape('sphere'); onSpawn('sphere'); }}>⚽ Sphere</button>
                        <button className={`spawn-btn ${selectedShape === 'cylinder' ? 'active' : ''}`} onClick={() => { setSelectedShape('cylinder'); onSpawn('cylinder'); }}>🛢️ Cylinder</button>
                    </div>
                    <button className="clear-btn" onClick={onClear}>🗑️ Clear All</button>
                </div>
            </div>
        )}
      </div>
    );
};
