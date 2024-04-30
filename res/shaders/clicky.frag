#version 460

layout(location = 0) flat in uint PO_index;

layout(location = 0) out float color;

void main() 
{	
	color = uintBitsToFloat(PO_index);
}