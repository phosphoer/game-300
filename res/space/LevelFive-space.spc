<Space>
   
  <GameSystem>
  </GameSystem>

  <ObjectManager>
    <ObjectLayerName>Objects</ObjectLayerName>
    <ObjectsToCreate>
      game-hotkeys.bp
    </ObjectsToCreate>
  </ObjectManager>

  <InputSystem>
    <ActionMap>actions_normal_directional.acn</ActionMap>
    <ActionMap>actions_southpaw_directional.acn</ActionMap>
    <ActionMap>actions_normal_positional.acn</ActionMap>
    <ActionMap>actions_southpaw_positional.acn</ActionMap>
  </InputSystem>

  <SoundSystem>    
  </SoundSystem>
  
  <TileSystem>
    <World>LevelFive-level.wld</World>
    <StartAxis>0</StartAxis>
    <LevelNumber>6</LevelNumber>
  </TileSystem>

  <Physics>
    <CollisionLayerName>Collision</CollisionLayerName>
    <Gravity>
      <X>0.0</X> <Y>-35.0</Y> <Z>0.0</Z>
    </Gravity>
    <MaxVelocity>17.5</MaxVelocity>
    <TileDebugDrawDistance>10</TileDebugDrawDistance>
  </Physics>

  <LogicSystem>
  </LogicSystem>

  <GraphicsSystem>
    <VSync>false</VSync>
    <ClearColor><X>0.05</X><Y>0.05</Y><Z>0.05</Z><W>1.0</W></ClearColor>
    <AnchorFlattenTime>1.0</AnchorFlattenTime>
  </GraphicsSystem>

  <HUDSystem>
    <HUDItem>
      <Name>RotationPrompt</Name>
      <Position>
        <X>47</X>
        <Y>55</Y>
      </Position>
      <CSSAttr>textonly centertext</CSSAttr>
      <Text>NULL</Text>
    </HUDItem>
    <HUDItem>
      <Name>NarrationPrompt</Name>
      <Position>
        <X>35</X>
        <Y>60</Y>
      </Position>
      <CSSAttr>textonly centertext narration</CSSAttr>
      <Text>NULL</Text>
    </HUDItem>
  </HUDSystem>

</Space>
