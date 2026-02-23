#ifndef CONTAINERUI_WEBCONTENT_HPP
#define CONTAINERUI_WEBCONTECT_HPP

namespace container_ui
{

constexpr const char index[] = R"(<!DOCTYPE html>
<html>
<head>
    <title>Container UI</title>
    <style type="text/css">
        html, body {
            font-family: Courier New, Monospace;
        }

        tr {
            text-align: left;
        }

        td {
            text-align: right;
        }

        table, tr, th, td {
            border: 1px solid black;
        }

        th, td {
            padding: 5px;
        }

        table {
            border-collapse: collapse;
        }
    </style>
    <script type="text/javascript">

    async function fetch_json(url) {
        const response = await fetch(url);
        return await response.json();
    }

    function format_mem(value) {
        if (value < 1024) {
            return `${value} Bytes`;
        }
        value /= 1024;

        if (value < 1024) {
            return `${value.toFixed(1)} KBytes`;
        }
        value /= 1024;
        
        if (value < 1024) {
            return `${value.toFixed(1)} MBytes`;
        }
        value /= 1024;

        return `${value.toFixed(1)} GBytes`;
    }

    function set_text(id, value) {
        const element = document.querySelector(id);
        element.textContent = value;

    }

    async function engine_info() {
        const data = await fetch_json("api/version");
        set_text("#engine_name", data.Platform.Name);
        set_text("#engine_version", data.Version);
    }

    async function system_info() {
        const data = await fetch_json("api/info");
        set_text("#system_os", data.OperatingSystem);
        set_text("#system_arch", data.Architecture);
        set_text("#system_ncpu", data.NCPU);
        set_text("#system_memory", format_mem(data.MemTotal));
    }

    async function disk_usage() {
        const data = await fetch_json("api/system/df");

        set_text("#df_container_count", data.ContainerUsage.TotalCount);
        set_text("#df_container_total", format_mem(data.ContainerUsage.TotalSize));
        set_text("#df_container_reclaimable", format_mem(data.ContainerUsage.Reclaimable));

        set_text("#df_image_count", data.ImageUsage.TotalCount);
        set_text("#df_image_total", format_mem(data.ImageUsage.TotalSize));
        set_text("#df_image_reclaimable", format_mem(data.ImageUsage.Reclaimable));

        set_text("#df_volume_count", data.VolumeUsage.TotalCount);
        set_text("#df_volume_total", format_mem(data.VolumeUsage.TotalSize));
        set_text("#df_volume_reclaimable", format_mem(data.VolumeUsage.Reclaimable));

        set_text("#df_bcache_count", data.BuildCacheUsage.TotalCount);
        set_text("#df_bcache_total", format_mem(data.BuildCacheUsage.TotalSize));
        set_text("#df_bcache_reclaimable", format_mem(data.BuildCacheUsage.Reclaimable));
    }


    async function containers() {
        const data = await fetch_json("api/containers/json");
        const element = document.querySelector("#containers");

        for(const container of data) {
            const tr = document.createElement("tr");
            element.appendChild(tr);

            const id = document.createElement("td");
            id.textContent = container.Id;
            tr.appendChild(id);

            const names = document.createElement("td");
            names.textContent = container.Names.map((name) => name.substring(1)).join(" ");
            tr.appendChild(names);

            const image = document.createElement("td");
            image.textContent = container.Image;
            tr.appendChild(image);

            const status = document.createElement("td");
            status.textContent = container.Status;
            tr.appendChild(status);

            const ports = document.createElement("td");
            ports.textContent = container.Ports.join(" ");
            tr.appendChild(ports);
        }    
    }

    async function images() {
        const data = await fetch_json("api/images/json");
        const element = document.querySelector("#images");

        for(const image of data) {
            const tr = document.createElement("tr");
            element.appendChild(tr);

            const id = document.createElement("td");
            id.textContent = image.Id;
            tr.appendChild(id);

            const tags = document.createElement("td");
            tags.textContent = image.RepoTags.join(" ");
            tr.appendChild(tags);

            const created = document.createElement("td");
            created.textContent = new Date(image.Created * 1000).toISOString();
            tr.appendChild(created);

            const size = document.createElement("td");
            size.textContent = format_mem(image.Size);
            tr.appendChild(size);

        }
    }

    async function volumes() {
        const data = await fetch_json("api/volumes");
        const element = document.querySelector("#volumes");

        for(const volume of data.Volumes) {
            const tr = document.createElement("tr");
            element.appendChild(tr);

            const name = document.createElement("td");
            name.textContent = volume.Name;
            tr.appendChild(name);

            const created = document.createElement("td");
            created.textContent = volume.CreatedAt;
            tr.appendChild(created);
        }
    }


    async function startup() {
        engine_info();
        system_info();
        disk_usage();
        containers();
        images();
        volumes();
    }


    </script>
</head>
<body onload="startup();">
    <h1>Container UI</h1>
    
    <h2>System</h2>
    <table>
        <tr><th>Operating System</th><td id="system_os"></td></tr>
        <tr><th>Architecture</th><td id="system_arch"></td></tr>
        <tr><th>CPUs</th><td id="system_ncpu"></td></tr>
        <tr><th>Memory</th><td id="system_memory"></td></tr>
    </table>


    <h2>Engine</h2>
    <table>
        <tr><th>Name</th><td id="engine_name"></td></tr>
        <tr><th>Version</th><td id="engine_version"></td></tr>
    </table>  

    <h2>Disk Usage</h2>
    <table>
        <tr><th>Category</th><th>Count</th><th>Total Size</th><th>Reclaimable Size</th></tr>
        <tr><th>Containers</th><td id="df_container_count"></td><td id="df_container_total"></td><td id="df_container_reclaimable"></td></tr>
        <tr><th>Images</th><td id="df_image_count"></td><td id="df_image_total"></td><td id="df_image_reclaimable"></td></tr>
        <tr><th>Volumes</th><td id="df_volume_count"></td><td id="df_volume_total"></td><td id="df_volume_reclaimable"></td></tr>
        <tr><th>Build Cache</th><td id="df_bcache_count"></td><td id="df_bcache_total"></td><td id="df_bcache_reclaimable"></td></tr>
    </table>

    <h2>Containers</h2>
    <table>
        <thead>
            <tr>
                <th>ID</th>
                <th>Names</th>
                <th>Image</th>
                <th>Status</th>
                <th>Ports</th>
            </tr>
        </thead>
        <tbody id="containers"></tbody>
    </table>

    <h2>Images</h2>
    <table>
        <thead>
            <tr>
                <th>ID</th>
                <th>Tags</th>
                <th>Created</th>
                <th>Size</th>
            </tr>
        </thead>
        <tbody id="images"></tbody>
    </table>

    <h2>Volumes</h2>
    <table>
        <thead>
            <tr>
                <th>Name</th>
                <th>Created</th>
            </tr>
        </thead>
        <tbody id="volumes"></tbody>
    </table>

</body>
</html>)";

}


#endif
