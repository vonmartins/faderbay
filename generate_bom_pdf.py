#!/usr/bin/env python3
import csv
from reportlab.lib.pagesizes import A4
from reportlab.lib import colors
from reportlab.lib.units import cm
from reportlab.platypus import SimpleDocTemplate, Table, TableStyle, Paragraph, Spacer
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_CENTER

# Read CSV file
data = []
with open('Faderbay.csv', 'r') as f:
    reader = csv.reader(f)
    for row in reader:
        data.append(row)

# Create PDF
pdf_file = 'Faderbay_BOM.pdf'
doc = SimpleDocTemplate(
    pdf_file,
    pagesize=A4,
    rightMargin=1*cm,
    leftMargin=1*cm,
    topMargin=1.5*cm,
    bottomMargin=1*cm,
)

# Container for the 'Flowable' objects
elements = []

# Add title
styles = getSampleStyleSheet()
title_style = ParagraphStyle(
    'CustomTitle',
    parent=styles['Heading1'],
    fontSize=16,
    textColor=colors.HexColor('#1f4788'),
    spaceAfter=12,
    alignment=TA_CENTER
)
title = Paragraph("FaderBay - Bill of Materials", title_style)
elements.append(title)
elements.append(Spacer(1, 0.3*cm))

# Create table with data
table = Table(data, repeatRows=1)

# Add style to table
table.setStyle(TableStyle([
    # Header row
    ('BACKGROUND', (0, 0), (-1, 0), colors.HexColor('#1f4788')),
    ('TEXTCOLOR', (0, 0), (-1, 0), colors.whitesmoke),
    ('ALIGN', (0, 0), (-1, -1), 'LEFT'),
    ('FONTNAME', (0, 0), (-1, 0), 'Helvetica-Bold'),
    ('FONTSIZE', (0, 0), (-1, 0), 8),
    ('BOTTOMPADDING', (0, 0), (-1, 0), 8),
    ('TOPPADDING', (0, 0), (-1, 0), 8),
    
    # Data rows
    ('FONTNAME', (0, 1), (-1, -1), 'Helvetica'),
    ('FONTSIZE', (0, 1), (-1, -1), 6),
    ('ROWBACKGROUNDS', (0, 1), (-1, -1), [colors.white, colors.HexColor('#f0f0f0')]),
    ('GRID', (0, 0), (-1, -1), 0.5, colors.grey),
    ('VALIGN', (0, 0), (-1, -1), 'MIDDLE'),
    ('LEFTPADDING', (0, 0), (-1, -1), 4),
    ('RIGHTPADDING', (0, 0), (-1, -1), 4),
    ('TOPPADDING', (0, 1), (-1, -1), 3),
    ('BOTTOMPADDING', (0, 1), (-1, -1), 3),
]))

elements.append(table)

# Build PDF
doc.build(elements)
print(f"PDF generado exitosamente: {pdf_file}")
print(f"Total de componentes: {len(data)-1}")
