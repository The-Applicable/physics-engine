#!/bin/sh
set -e

# Get the port from environment variable (Cloud Run sets this, default to 80)
PORT=${PORT:-80}

# Replace the port in nginx configuration
sed -i "s/listen 80;/listen ${PORT};/g" /etc/nginx/conf.d/default.conf

# Generate runtime config.js with environment variables
# This allows configuration to be injected at runtime instead of build time
cat > /usr/share/nginx/html/config.js <<EOF
// Runtime configuration injected at container startup
window.env = window.env || {};
EOF

# Inject the config.js script tag into index.html before other scripts
# This ensures the config is available before the app loads
if grep -q '<script src="/config.js"></script>' /usr/share/nginx/html/index.html; then
  echo "config.js already injected in index.html"
else
  # Find the head tag and insert config.js before the closing </head>
  sed -i 's|</head>|  <script src="/config.js"></script>\n  </head>|' /usr/share/nginx/html/index.html
fi

# Start nginx
exec nginx -g "daemon off;"
